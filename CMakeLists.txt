cmake_minimum_required(VERSION 3.9)
project(joinjs)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory("src/main")
add_subdirectory("src/test")

enable_testing()
include(CTest)