#TODO rename folder to common
add_library(
  libcommon
  SCommand.h
  SCommand.cpp
  Types.h
  Types.cpp
  Opcodes.def)

target_link_libraries(
  libcommon
 PRIVATE
  liblog)