#include "Import.h"

#include <assert.h>
#include <windows.h>
#include <map>
#include <ostream>

namespace NPeProtector {
namespace {
typedef std::pair<int /*index in commands*/, std::string /*function name*/>
    tIndexToFunction;
typedef std::vector<tIndexToFunction> tFunctions;
typedef std::map<std::string, tFunctions> tImport;

typedef tFunctions::const_iterator tFunctionIterator;
typedef tImport::const_iterator tImportIterator;

tImport getImport(const std::vector<SCommand>& commands) {
  tImport result;
  for (unsigned int i = 0; i < commands.size(); ++i) {
    if (commands[i].mType == NCommand::IMPORT) {
      result[commands[i].mImport.mDllName].push_back(
          tIndexToFunction(i, commands[i].mImport.mFunctionName));
    }
  }
  return result;
}

// total amount of functions!
int getNumberFunctions(const tImport& import) {
  int result = 0;
  for (tImportIterator i = import.begin(); i != import.end(); ++i) {
    for (tFunctionIterator j = i->second.begin(); j != i->second.end(); ++j) {
      result += 1;
    }
  }
  return result;
}

uint32_t toRVA(uint8_t* pointer, uint8_t* base, uint32_t importRVA) {
  return (pointer - base) + importRVA;
}
}  // namespace

int getImportSize(const std::vector<SCommand>& commands) {
  const tImport& import = getImport(commands);

  int size = 0;
  for (tImportIterator i = import.begin(); i != import.end(); ++i) {
    size += sizeof(IMAGE_IMPORT_DESCRIPTOR);
    size += i->first.length() + 1;
    size += ((i->second.size() + 1 /*end*/) * sizeof(uint32_t)) *
            2 /*OriginalFirstThunk + FirstThunk*/;
    for (tFunctionIterator j = i->second.begin(); j != i->second.end(); ++j) {
      size += j->second.length() + 1 /*zero byte*/ +
              sizeof(uint32_t) /*IMAGE_IMPORT_BY_NAME::Hint*/;
    }
  }
  size += sizeof(IMAGE_IMPORT_DESCRIPTOR) /*end*/;

  return size;
}

void resolveImport(std::vector<SCommand>& commands, const uint32_t importRVA) {
  const tImport& import = getImport(commands);

  const int dllNumber = import.size();
  const int descriptionSize = (dllNumber + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
  const int originalFirstChunkSize =
      (getNumberFunctions(import) + dllNumber) * sizeof(uint32_t);

  uint32_t firstChunkRVA = importRVA + descriptionSize + originalFirstChunkSize;

  for (tImportIterator i = import.begin(); i != import.end(); ++i) {
    for (tFunctionIterator j = i->second.begin(); j != i->second.end(); ++j) {
      assert(commands[j->first].mType == NCommand::IMPORT);
      commands[j->first].mRVA = firstChunkRVA;
      firstChunkRVA += sizeof(uint32_t);
    }
    firstChunkRVA += sizeof(uint32_t);
  }
}

void putImport(std::ostream& output,
               const std::vector<SCommand>& commands,
               const uint32_t importRVA) {
  const tImport& import = getImport(commands);
  /*
   IMAGE_IMPORT_DESCRIPTOR(1)
   IMAGE_IMPORT_DESCRIPTOR(2)
   IMAGE_IMPORT_DESCRIPTOR(end)
   originalFirstChunk(1)(1)
   originalFirstChunk(1)(2)
   originalFirstChunk(1)(3)
   originalFirstChunk(1)(end)
   originalFirstChunk(2)(1)
   originalFirstChunk(2)(2)
   originalFirstChunk(2)(end)
   firstChunk(1)(1)
   firstChunk(1)(2)
   firstChunk(1)(3)
   firstChunk(1)(end)
   firstChunk(2)(1)
   firstChunk(2)(2)
   firstChunk(2)(end)
   dllName(1)
   functionName(1)(1)
   functionName(1)(2)
   functionName(1)(3)
   dllName(2)
   functionName(2)(1)
   functionName(2)(2)
   **/

  const int dllNumber = import.size();
  const int descriptionSize = (dllNumber + 1) * sizeof(IMAGE_IMPORT_DESCRIPTOR);
  const int originalFirstChunkSize =
      (getNumberFunctions(import) + dllNumber) * sizeof(uint32_t);
  const int firstChunkSize = originalFirstChunkSize;

  const int importSize = getImportSize(commands);

  char* const buffer = new char[importSize];
  memset(buffer, 0, importSize);

  IMAGE_IMPORT_DESCRIPTOR* descriptionPtr =
      reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(buffer);
  uint32_t* originalFirstChunkPtr =
      reinterpret_cast<uint32_t*>(buffer + descriptionSize);
  uint32_t* firstChunkPtr = reinterpret_cast<uint32_t*>(
      buffer + descriptionSize + originalFirstChunkSize);
  char* namesPtr = reinterpret_cast<char*>(
      buffer + descriptionSize + originalFirstChunkSize + firstChunkSize);

  for (tImportIterator i = import.begin(); i != import.end(); ++i) {
    descriptionPtr->OriginalFirstThunk =
        toRVA(reinterpret_cast<uint8_t*>(originalFirstChunkPtr),
              reinterpret_cast<uint8_t*>(buffer), importRVA);
    descriptionPtr->FirstThunk =
        toRVA(reinterpret_cast<uint8_t*>(firstChunkPtr),
              reinterpret_cast<uint8_t*>(buffer), importRVA);
    descriptionPtr->Name = toRVA(reinterpret_cast<uint8_t*>(namesPtr),
                                 reinterpret_cast<uint8_t*>(buffer), importRVA);

    // copy dll name
#pragma warning(push)
#pragma warning(disable : 4996)
    strcpy(namesPtr, i->first.c_str());
#pragma warning(pop)

    namesPtr += strlen(i->first.c_str()) + 1;

    for (tFunctionIterator j = i->second.begin(); j != i->second.end(); ++j) {
      *originalFirstChunkPtr =
          toRVA(reinterpret_cast<uint8_t*>(namesPtr),
                reinterpret_cast<uint8_t*>(buffer), importRVA);
      *firstChunkPtr = toRVA(reinterpret_cast<uint8_t*>(namesPtr),
                             reinterpret_cast<uint8_t*>(buffer), importRVA);
      // copy hint
      *(namesPtr + 0) = 0;
      *(namesPtr + 1) = 0;

#pragma warning(push)
#pragma warning(disable : 4996)
      strcpy(namesPtr + 2,
             /*importSize - ((namesPtr + 2)- buffer),*/ j->second.c_str());
#pragma warning(pop)

      namesPtr += strlen(j->second.c_str()) + 1 + 2;

      originalFirstChunkPtr++;
      firstChunkPtr++;
    }
    descriptionPtr++;

    // terminated zeros;
    originalFirstChunkPtr++;
    firstChunkPtr++;
  }

  output.write(buffer, importSize);

  delete[] buffer;
}
}  // namespace NPeProtector
