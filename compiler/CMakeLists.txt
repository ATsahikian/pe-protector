
add_library(
  libcompiler
  CCompile.h
  CCompile.cpp
  CLexicalAnalizer.h
  CLexicalAnalizer.cpp)

target_link_libraries(
  libcompiler
 PUBLIC
  libcommon
 PRIVATE
  liblog)

add_executable(
  compiler
  main.cpp)

target_link_libraries(
  compiler
 PRIVATE
  libcompiler
  liblog)
