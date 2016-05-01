add_library(
  libcommon
  SCommand.h
  SCommand.cpp
  Types.h
  Types.cpp
  Opcodes.def)

target_include_directories(
  libcommon
 PUBLIC
  "${CMAKE_SOURCE_DIR}")

target_link_libraries(
  libcommon
 PRIVATE
  liblog)