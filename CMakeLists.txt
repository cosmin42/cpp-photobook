cmake_minimum_required(VERSION 3.27)

SET(CMAKE_CXX_FLAGS "-std=c++20")

project(PhotoBook)

add_subdirectory(PB)

add_executable(PhotoBook main.cpp)