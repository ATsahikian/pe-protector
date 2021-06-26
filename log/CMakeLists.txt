
add_library(
  liblog
  CLog.h
  CLog.cpp)

target_include_directories(
  liblog
 PUBLIC
  "${CMAKE_SOURCE_DIR}")
