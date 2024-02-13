cmake_minimum_required(VERSION 3.27)

project(PhotoBook)

if (MSVC)
    add_compile_options(/std:c++20)
    add_compile_definitions(NOMINMAX)
else()
    set(CMAKE_CXX_FLAGS "-std=c++20")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

set(PBLIB_LIBRARY_NAME pblib-${CMAKE_GENERATOR_PLATFORM})

find_package(thread-pool CONFIG REQUIRED)

include_directories(PB/third-party/podofo-build/include)

add_subdirectory(PB)

add_executable(PhotoBook
    include/MainActivity.h
    src/ArgsHandler.cpp
    src/PBArgsListener.cpp
    main.cpp)

set_target_properties(PhotoBook PROPERTIES VS_GLOBAL_VcpkgEnabled true)

target_include_directories(PhotoBook PRIVATE PB/include include build-${CMAKE_GENERATOR_PLATFORM}/PB/include vcpkg/installed/${CMAKE_GENERATOR_PLATFORM}-windows/include/thread-pool-0.6.2)

target_link_libraries(PhotoBook PRIVATE ${PBLIB_LIBRARY_NAME})