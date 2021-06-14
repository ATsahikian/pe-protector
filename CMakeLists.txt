cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(pe-protector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(PeProtector)
add_subdirectory(Compile)
add_subdirectory(Library)
add_subdirectory(LogLibrary)
add_subdirectory(Test)
