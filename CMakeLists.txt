cmake_minimum_required(VERSION 3.27)

project(PhotoBook)

if (MSVC)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
    add_compile_definitions(NOMINMAX)
else()
    set(CMAKE_CXX_FLAGS "-std=c++20")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()
include_directories(PB/third-party/podofo-build/include)

add_subdirectory(PB)

add_executable(PhotoBook
    include/MainActivity.h
    src/ArgsHandler.cpp
    src/PBArgsListener.cpp
    main.cpp)

target_include_directories(PhotoBook PUBLIC PB/include include)

target_link_libraries(PhotoBook pblib)