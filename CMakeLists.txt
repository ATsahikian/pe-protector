cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(pe-protector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#set(CMAKE_SYSTEM_PROCESSOR "i686")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32  -fno-permissive" CACHE STRING "c++ flags")
#set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -m32" CACHE STRING "c flags")

enable_testing()

add_subdirectory(pe-protector)
add_subdirectory(compiler)
add_subdirectory(common)
add_subdirectory(log)
add_subdirectory(test)
add_subdirectory(examples)
