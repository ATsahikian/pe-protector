cmake_minimum_required(VERSION 3.17 FATAL_ERROR)
project(pe-protector VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_testing()

add_subdirectory(pe-protector)
add_subdirectory(compiler)
add_subdirectory(common)
add_subdirectory(log)
add_subdirectory(test)
