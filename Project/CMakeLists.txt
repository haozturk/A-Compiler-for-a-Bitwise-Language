cmake_minimum_required(VERSION 3.8)
project(cmpe230_P1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp Token.cpp Token.h)
add_executable(cmpe230_P1 ${SOURCE_FILES})
