
find_package(range-v3 CONFIG REQUIRED)
find_package(Boost REQUIRED)

add_library(
  libcompiler
  Parser.h
  Parser.cpp)

target_link_libraries(
  libcompiler
 PUBLIC
  libcommon
 PRIVATE
  liblog
  range-v3 
  range-v3-meta 
  range-v3::meta 
  range-v3-concepts
  Boost::boost)

target_include_directories(
  libcompiler
 PUBLIC
  "${CMAKE_SOURCE_DIR}")

add_executable(
  compiler
  main.cpp)

target_link_libraries(
  compiler
 PRIVATE
  libcompiler
  liblog)
