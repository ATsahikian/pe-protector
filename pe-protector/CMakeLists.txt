# TODO extract libpe for pe parser
add_library(
  libpe-protector
  ClientFile.h
  ClientFile.cpp
  Data.h
  Data.cpp
  Import.h
  Import.cpp
  Instruction.h
  Instruction.cpp
  InstructionModRM.h
  InstructionModRM.cpp
  Mutation.h
  Mutation.cpp
  Opcodes.h
  Opcodes.cpp
  PeHeader.h
  PeHeader.cpp
  ProtectPe.cpp
  ProtectPe.h
  Resources.h
  Resources.cpp)

target_link_libraries(
  libpe-protector
 PUBLIC
  libcommon
 PRIVATE
  liblog
  Dbghelp
  "${CMAKE_SOURCE_DIR}/aplib/aplib.lib")

target_link_options(
  libpe-protector
 PUBLIC
  "/SAFESEH:NO")

add_executable(
  pe-protector
  main.cpp
  pe-protector.rc
  resource.h)

#TODO: asm file, bin file, custor rule to compile asm to bin
target_link_libraries(
  pe-protector
 PRIVATE
  libpe-protector
  liblog)
