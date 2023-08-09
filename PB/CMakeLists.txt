cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_FLAGS "-std=c++20")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")


option(STATIC_CHECK "Statically check the program." OFF)

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

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