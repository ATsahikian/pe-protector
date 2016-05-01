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

target_include_directories(
  libpe-protector
 PUBLIC
  "${CMAKE_SOURCE_DIR}")

#target_link_options(
#  libpe-protector
 #PUBLIC
#  "/SAFESEH:NO")

set(PE_PROTECTOR_STUB_BIN "pe-protector-stub.bin")

add_executable(
  pe-protector
  main.cpp
  pe-protector.rc
  resource.h
  ${PE_PROTECTOR_STUB_BIN})

target_compile_definitions(
  pe-protector
 PRIVATE
  PE_PROTECTOR_STUB_BIN="${PE_PROTECTOR_STUB_BIN}")

target_link_libraries(
  pe-protector
 PRIVATE
  libpe-protector
  liblog)

add_custom_command(
 OUTPUT ${PE_PROTECTOR_STUB_BIN}
 COMMAND
   compiler
   ${CMAKE_SOURCE_DIR}/data/pe-protector-stub.asm
   ${PE_PROTECTOR_STUB_BIN}
 DEPENDS
  compiler
  ${CMAKE_SOURCE_DIR}/data/pe-protector-stub.asm)
