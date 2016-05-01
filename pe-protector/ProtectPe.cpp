#include "ProtectPe.h"

#include "ClientFile.h"
#include "Data.h"
#include "Import.h"
#include "Instruction.h"
#include "Mutation.h"
#include "PeHeader.h"
#include "Resources.h"
#include "resource.h"

#include "common/SCommand.h"
#include "log/CLog.h"

#include <windef.h>

#include <windows.h>
#include <cassert>
#include <ostream>
#include <sstream>  // std::stringbuf
#include <string>   // std::string
#include <vector>

namespace NPeProtector {
// TODO move this
inline int align(int value, int al) {
  if (value % al) {
    value += al - (value % al);
  }
  return value;
}
namespace {
int getDirectiveSize(SDirective& directive,
                     const std::vector<SCommand>& commands,
                     const SClientFile& clientFile) {
  if (directive.mName == "IMPORT_DIRECTORY") {
    directive.mDirectorySize = getImportSize(commands);
    return directive.mDirectorySize;
  } else if (directive.mName == "RECOURCE_DIRECTORY") {
    directive.mDirectorySize = getResourcesSize(clientFile);
    return directive.mDirectorySize;
  } else if (directive.mName == "COMPRESSED_FILE") {
    return clientFile.mCompressed.size();
  } else {
    throw std::runtime_error(
        ("Failed to get directive " + directive.mName).c_str());
  }
  return 0;
}

void putDirective(std::ostream& output,
                  const std::vector<SCommand>& commands,
                  const SClientFile& clientFile,
                  const SDirective& directive,
                  const DWORD baseRVA) {
  if (directive.mName == "IMPORT_DIRECTORY") {
    putImport(output, commands, baseRVA);
  } else if (directive.mName == "RECOURCE_DIRECTORY") {
    putResources(output, commands, clientFile, baseRVA);
  } else if (directive.mName == "COMPRESSED_FILE") {
    if (!clientFile.mCompressed.empty()) {
      output.write(&clientFile.mCompressed.front(),
                   clientFile.mCompressed.size());
    }
  } else {
    throw std::runtime_error(
        ("Failed to get directive " + directive.mName).c_str());
  }
}

void resolveLabels(std::vector<SCommand>& commands,
                   const SClientFile& clientFile) {
  int nextRVA = align(getHeaderSize(), gSectionAlignment);
  int nextRAW = align(getHeaderSize(), gFileAlignment);
  int size = 0;
  for (unsigned int i = 0; i < commands.size(); ++i) {
    switch (commands[i].mType) {
      case NCommand::DIRECTIVE: {
        commands[i].mRVA = nextRVA;
        commands[i].mRAW = nextRAW;

        if (!_strcmpi(commands[i].mDirective.mName.c_str(),
                      "IMPORT_DIRECTORY")) {
          resolveImport(commands, commands[i].mRVA);
        }

        size = getDirectiveSize(commands[i].mDirective, commands, clientFile);
        nextRVA += size;
        nextRAW += size;
        break;
      }
      case NCommand::INSTRUCTION:
        commands[i].mRVA = nextRVA;
        commands[i].mRAW = nextRAW;
        size = getInstructionSize(commands[i].mInstruction);
        nextRVA += size;
        nextRAW += size;
        break;
      case NCommand::DATA:
        commands[i].mRVA = nextRVA;
        commands[i].mRAW = nextRAW;
        size = getDataSize(commands[i].mData);
        nextRVA += size;
        nextRAW += size;
        break;
      case NCommand::EXTERN:
      case NCommand::IMPORT:
        // skip
        break;
      case NCommand::SECTION:
      case NCommand::END:
        nextRVA = align(nextRVA, gSectionAlignment);
        nextRAW = align(nextRAW, gFileAlignment);
        commands[i].mRVA = nextRVA;
        commands[i].mRAW = nextRAW;
        break;
    }
  }
}
void putZeroBytes(std::ostream& output, const SCommand& command) {
  const int currentPosition = static_cast<int>(output.tellp());
  const int size = command.mRAW - currentPosition;

  assert(size >= 0);

  for (int i = 0; i < size; ++i) {
    char c = 0;
    output.write(&c, 1);
  }
}

void putBody(std::ostream& output,
             const std::vector<SCommand>& commands,
             const SClientFile& clientFile) {
  for (unsigned int i = 0; i < commands.size(); ++i) {
    switch (commands[i].mType) {
      case NCommand::DIRECTIVE:
        putDirective(output, commands, clientFile, commands[i].mDirective,
                     commands[i].mRVA);
        break;
      case NCommand::INSTRUCTION:
        putInstruction(output, commands[i].mInstruction, commands,
                       commands[i].mRVA);
        break;
      case NCommand::DATA:
        putData(output, commands[i].mData, commands);
        break;
      case NCommand::EXTERN:
      case NCommand::IMPORT:
        break;
      case NCommand::SECTION:  // section begin !
      case NCommand::END:
        putZeroBytes(output, commands[i]);
        break;
    }
  }
}

void setExterns(std::vector<SCommand>& commands,
                const SClientFile& clientFile) {
  for (unsigned int i = 0; i < commands.size(); ++i) {
    if (commands[i].mType == NCommand::EXTERN) {
      if (commands[i].mNameLabel == "externImageBase") {
        SConstant constant;
        constant.mValue = clientFile.mImageBase;
        commands[i].mData.mConstants.push_back(constant);
      } else if (commands[i].mNameLabel == "externImageSize") {
        SConstant constant;
        constant.mValue = clientFile.mImageSize;
        commands[i].mData.mConstants.push_back(constant);
      } else if (commands[i].mNameLabel == "externOEP") {
        SConstant constant;
        constant.mValue = clientFile.mOEP;
        commands[i].mData.mConstants.push_back(constant);
      }
    }
  }
}

std::vector<SCommand> loadCommands() {
  std::vector<SCommand> commands;

  const HRSRC rsrcHandle = ::FindResource(
      NULL, MAKEINTRESOURCE(RESOURCE_IDENTIFIER_COMMANDS), RT_RCDATA);
  if (rsrcHandle != 0) {
    const DWORD rsrcRawSize = ::SizeofResource(NULL, rsrcHandle);
    if (rsrcRawSize != 0) {
      const HGLOBAL rsrcPtr = ::LoadResource(NULL, rsrcHandle);
      if (rsrcPtr != 0) {
        std::istringstream inputRsrc(std::string((char*)rsrcPtr, rsrcRawSize));

        deserialize(inputRsrc, commands);
      }
    }
  }
  if (commands.empty()) {
    throw std::runtime_error("Failed to load resources");
  }
  return commands;
}
}  // namespace

void protectPe(std::ostream& output, const SClientFile& clientFile) {
  // get commands
  std::vector<SCommand> commands = loadCommands();

  // set externs
  setExterns(commands, clientFile);
  LOG_DEBUG("before:");
  loggingCommands(commands);
  // mutateCommands(commands);
  // mutateCommands(commands);
  mutateCommands(commands);
  mutateCommands(commands);

  LOG_DEBUG("after:");
  loggingCommands(commands);

  // resolveLabels - done
  resolveLabels(commands, clientFile);

  LOG_DEBUG("Resolved:");
  loggingCommands(commands);

  putHeader(output, commands, clientFile);
  putBody(output, commands, clientFile);
}
}  // namespace NPeProtector
