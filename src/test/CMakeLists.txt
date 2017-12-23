cmake_minimum_required(VERSION 3.9)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(joinjstest joinjstest.cpp ../main/exceptions.h)

target_link_libraries(joinjstest joinjs)

add_test(NAME JsonJSTest COMMAND Test)

include_directories(/usr/local/lib/boost)

#add_subdirectory(../test ../main)