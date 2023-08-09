cmake_minimum_required(VERSION 3.27)

SET(CMAKE_CXX_FLAGS "-std=c++20")

project(pblib)

add_library(pblib SHARED
    include/PBLog.h
    include/util/Traits.h
)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(third-party/googletest)

add_executable(PBTests tests/testLogLevel.cpp )

target_include_directories(PBTests PRIVATE include)

target_link_libraries(PBTests gtest gtest_main)