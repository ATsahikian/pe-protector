#include "PeHeader.h"

#include "common/SCommand.h"
#include "common/Types.h"

#include <assert.h>
#include <string.h>
#include <windows.h>
#include <sstream>
#include <vector>

namespace NPeProtector {
namespace {
struct SMzHeader {
  IMAGE_DOS_HEADER mHeader;
  uint8_t mDosStub[9];
  uint8_t mDosMessage[47 + 8];
};

const SMzHeader sMzHeader = {
    {
        /*e_magic    */ 0x5a4d,  // Magic number
        /*e_cblp     */ 0x0090,  // Bytes on last page of file
        /*e_cp       */ 0x0003,  // Pages in file
        /*e_crlc     */ 0x0000,  // Relocations
        /*e_cparhdr  */ 0x0004,  // Size of header in paragraphs
        /*e_minalloc */ 0x0000,  // Minimum extra paragraphs needed
        /*e_maxalloc */ 0xffff,  // Maximum extra paragraphs needed
        /*e_ss       */ 0x0000,  // Initial (relative) SS value
        /*e_sp       */ 0x00b8,  // Initial SP value
        /*e_csum     */ 0x0000,  // Checksum
        /*e_ip       */ 0x0000,  // Initial IP value
        /*e_cs       */ 0x0000,  // Initial (relative) CS value
        /*e_lfarlc   */ 0x0040,  // File address of relocation table
        /*e_ovno     */ 0x0000,  // Overlay number
        /*e_res[4]   */ 0,
        0,
        0,
        0,                  // Reserved words
        /*e_oemid    */ 0,  // OEM identifier (for e_oeminfo)
        /*e_oeminfo  */ 0,  // OEM information; e_oemid specific
        /*e_res2[10] */ 0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,                          // Reserved words
        /*e_lfanew   */ 0x00000080  // File address of new exe header
    },
    {0x0e, 0x1f, 0xba, 0x0e, 0x00, 0xb4, 0x09, 0xcd, 0x21},
    {'T',  'h',  'i',  's',  ' ',  'p',  'r',  'o',  'g',  'r',  'a',
     'm',  ' ',  'c',  'a',  'n',  'n',  'o',  't',  ' ',  'b',  'e',
     ' ',  'r',  'u',  'n',  ' ',  'i',  'n',  ' ',  'D',  'O',  'S',
     ' ',  'm',  'o',  'd',  'e',  0x2e, 0x0d, 0x0d, 0x0a, 0x24, 0x00,
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

IMAGE_NT_HEADERS32 sPeHeader = {
    /*PE*/ 0x00004550,
    {/*Machine               */ IMAGE_FILE_MACHINE_I386,
     /*NumberOfSections      */ 0,
     /*TimeDateStamp         */ 0,
     /*PointerToSymbolTable  */ 0,
     /*NumberOfSymbols       */ 0,
     /*SizeOfOptionalHeader  */ sizeof(IMAGE_OPTIONAL_HEADER32),
     /*Characteristics       */ IMAGE_FILE_EXECUTABLE_IMAGE |
         IMAGE_FILE_LINE_NUMS_STRIPPED | IMAGE_FILE_LOCAL_SYMS_STRIPPED |
         IMAGE_FILE_DEBUG_STRIPPED | IMAGE_FILE_32BIT_MACHINE},
    {/*Magic                   */ IMAGE_NT_OPTIONAL_HDR32_MAGIC,
     /*MajorLinkerVersion      */ 0x05,
     /*MinorLinkerVersion      */ 0x00,
     /*SizeOfCode              */ 0,
     /*SizeOfInitializedData   */ 0,
     /*SizeOfUninitializedData */ 0,
     /*AddressOfEntryPoint     */ 0x1000,
     /*BaseOfCode              */ 0x1000,
     /*BaseOfData              */ 0,
     /*ImageBase               */ 0x400000,
     /*SectionAlignment        */ 0x00001000,
     /*FileAlignment           */ 0x00000200,
     /*MajorOperatingSystemVersion*/ 0x0004,
     /*MinorOperatingSystemVersion*/ 0x0000,
     /*MajorImageVersion       */ 0x0000,
     /*MinorImageVersion       */ 0x0000,
     /*MajorSubsystemVersion   */ 0x0004,
     /*MinorSubsystemVersion   */ 0x0000,
     /*Win32VersionValue       */ 0,
     /*SizeOfImage             */ 0,
     /*SizeOfHeaders           */ 0x400,
     /*CheckSum                */ 0x0,
     /*Subsystem               */ IMAGE_SUBSYSTEM_WINDOWS_CUI, /*CUI TODO
                                                                replace*/
     /*DllCharacteristics      */ 0,
     /*SizeOfStackReserve      */ 0x100000,
     /*SizeOfStackCommit       */ 0x1000,
     /*SizeOfHeapReserve       */ 0x100000,
     /*SizeOfHeapCommit        */ 0x1000,
     /*LoaderFlags             */ 0,
     /*NumberOfRvaAndSizes     */ 0x10}};

int findDirective(const std::vector<SCommand>& commands,
                  const std::string& name) {
  for (unsigned int i = 0; i < commands.size(); ++i) {
    if (commands[i].mType == NCommand::DIRECTIVE &&
        commands[i].mDirective.mName == name) {
      return i;
    }
  }
  return -1;
}

int findSection(const std::vector<SCommand>& commands, const int startIndex) {
  for (unsigned int i = startIndex + 1; i < commands.size(); ++i) {
    if (commands[i].mType == NCommand::SECTION ||
        commands[i].mType == NCommand::END) {
      return i;
    }
  }
  return -1;
}

IMAGE_SECTION_HEADER getSection(const std::vector<SCommand>& commands,
                                const int sectionIndex,
                                const uint32_t sizeOfImage) {
  int a = sizeof(IMAGE_NT_HEADERS32);
  int b = sizeof(IMAGE_NT_HEADERS32);
  int c = sizeof(IMAGE_FILE_HEADER);
  IMAGE_NT_HEADERS32* pp = 0;
  void* p1 = &pp->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
                  .VirtualAddress;

  IMAGE_SECTION_HEADER result = {0};
  strncpy_s((char*)result.Name, sizeof(result.Name),
            commands[sectionIndex].mSection.mName.c_str(), sizeof(result.Name));

  const int nextSection = findSection(commands, sectionIndex);

  assert(nextSection != -1);

  result.PointerToRawData = commands[sectionIndex].mRAW;
  result.VirtualAddress = commands[sectionIndex].mRVA;
  result.SizeOfRawData =
      commands[nextSection].mRAW - commands[sectionIndex].mRAW;
  if (commands[nextSection].mType == NCommand::END &&
      commands[nextSection].mRVA < sizeOfImage) {
    result.Misc.VirtualSize = sizeOfImage - commands[sectionIndex].mRVA;
  } else {
    result.Misc.VirtualSize =
        commands[nextSection].mRVA - commands[sectionIndex].mRVA;
  }

  result.Characteristics = 0;

  if (commands[sectionIndex].mSection.mAttributes & NSectionAttributes::READ) {
    result.Characteristics |= IMAGE_SCN_MEM_READ;
  }
  if (commands[sectionIndex].mSection.mAttributes & NSectionAttributes::WRITE) {
    result.Characteristics |= IMAGE_SCN_MEM_WRITE;
  }
  if (commands[sectionIndex].mSection.mAttributes &
      NSectionAttributes::EXECUTE) {
    result.Characteristics |= IMAGE_SCN_MEM_EXECUTE;
  }
  if (commands[sectionIndex].mSection.mAttributes & NSectionAttributes::CODE) {
    result.Characteristics |= IMAGE_SCN_CNT_CODE;
  }
  if (commands[sectionIndex].mSection.mAttributes &
      NSectionAttributes::INITIALIZED) {
    result.Characteristics |= IMAGE_SCN_CNT_INITIALIZED_DATA;
  }

  return result;
}

std::vector<IMAGE_SECTION_HEADER> getSections(
    const std::vector<SCommand>& commands,
    uint32_t SizeOfImage) {
  std::vector<IMAGE_SECTION_HEADER> sections;

  for (unsigned int i = 0; i < commands.size(); ++i) {
    if (commands[i].mType == NCommand::SECTION) {
      sections.push_back(getSection(commands, i, SizeOfImage));
    }
  }
  return sections;
}
}  // namespace

int gImageBase = 0;

int getHeaderSize() {
  return 0x400;  // TODO
}

void putHeader(std::ostream& output,
               const std::vector<SCommand>& commands,
               const SClientFile& clientFile) {
  const std::vector<IMAGE_SECTION_HEADER>& sections =
      getSections(commands, clientFile.mImageSize);

  if (sections.empty()) {
    throw std::runtime_error("No sections");
  }

  sPeHeader.FileHeader.NumberOfSections = sections.size();
  sPeHeader.OptionalHeader.SizeOfImage =
      sections.back().VirtualAddress + sections.back().Misc.VirtualSize;
  sPeHeader.OptionalHeader.ImageBase = clientFile.mImageBase;

  const int importPosition = findDirective(commands, "IMPORT_DIRECTORY");
  if (importPosition != -1) {
    sPeHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]
        .VirtualAddress = commands[importPosition].mRVA;
    sPeHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size =
        commands[importPosition].mDirective.mDirectorySize;
  }
  const int resourcePosition = findDirective(commands, "RECOURCE_DIRECTORY");
  if (resourcePosition != -1) {
    sPeHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE]
        .VirtualAddress = commands[resourcePosition].mRVA;
    sPeHeader.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE]
        .Size = commands[resourcePosition].mDirective.mDirectorySize;
  }

  output.write((char*)&sMzHeader, sizeof(sMzHeader));
  output.write((char*)&sPeHeader, sizeof(sPeHeader));

  for (unsigned int i = 0; i < sections.size(); ++i) {
    output.write((char*)&sections[i], sizeof(sections[0]));
  }
}
}  // namespace NPeProtector