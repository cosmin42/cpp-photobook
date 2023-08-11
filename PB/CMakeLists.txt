cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_FLAGS "-std=c++20")
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)

if (NOT LOG_LEVEL)
    set(LOG_LEVEL None)
endif()

set(LOG_LEVEL_STRING_LIST None Debug Info Warning Error)

if (LOG_LEVEL IN_LIST LOG_LEVEL_STRING_LIST)
    configure_file(Config.h.in ../include/Config.h)
else()
    message(FATAL_ERROR "${LOG_LEVEL} Not in the list of levels ${LOG_LEVEL_STRING_LIST}" )
endif()

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

project(pblib)

add_library(pblib SHARED
    include/Config.h
    include/DataNode.h
    include/Error.h
    include/FileMapper.h
    include/Log.h
    include/MetadataReader.h
    include/FileData.h
    include/FolderData.h
    include/util/Traits.h

    src/FileMapper.cpp
    src/FileData.cpp
    src/FolderData.cpp
    src/MetadataReader.cpp
)

target_include_directories(pblib PRIVATE include)
target_include_directories(pblib PRIVATE third-party/boost-extract)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/uuid/include)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/detail/include)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/random/include)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/integer/include)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/tti/include)
target_include_directories(pblib PRIVATE third-party/boost-extract/libs/function_types/include)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(third-party/googletest)
add_subdirectory(common-components)
add_subdirectory(third-party/exiv2)

include_directories(${CMAKE_BINARY_DIR})

add_executable(pbtests tests/testLogLevel.cpp tests/TestErrors.cpp tests/TestThirdPartyLinks.cpp)

target_include_directories(pbtests PRIVATE include)
target_include_directories(pbtests PRIVATE common-components/include)
target_include_directories(pbtests PRIVATE third-party/exiv2/include)
target_include_directories(pbtests PRIVATE third-party/boost-extract)
target_include_directories(pbtests PRIVATE third-party/boost-extract/libs/uuid/include)

target_link_libraries(pbtests gtest gtest_main common-components exiv2lib)