cmake_minimum_required(VERSION 3.27)

set(CMAKE_CXX_FLAGS "-std=c++20")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)
option(PRINTER_TYPE "The printer type." OFF)

# The default printer
if (NOT PRINTER_TYPE)
    set(PRINTER_TYPE StandardPrinter)
endif()

# The default log level
if (NOT LOG_LEVEL)
    set(LOG_LEVEL None)
endif()

set(LOG_LEVEL_STRING_LIST None Debug Info Warning Error)

if (LOG_LEVEL IN_LIST LOG_LEVEL_STRING_LIST)
    message("Generating config files...")
    configure_file(Config.h.in include/Config.h)
    configure_file(ConfigDependants.h.in include/ConfigDependants.h)
else()
    message(FATAL_ERROR "${LOG_LEVEL} Not in the list of levels ${LOG_LEVEL_STRING_LIST}" )
endif()

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

project(pblib)

add_subdirectory(third-party/magic_enum)

add_library(pblib SHARED
    include/common/Log.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/Config.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/ConfigDependants.h
    include/DataNode.h
    include/Error.h
    include/FileMapper.h
    include/MetadataReader.h
    include/FileData.h
    include/FolderData.h
    include/util/Traits.h

    src/FileMapper.cpp
    src/FileData.cpp
    src/FolderData.cpp
    src/MetadataReader.cpp
)

target_include_directories(pblib PRIVATE include
    third-party/boost-extract
    third-party/boost-extract/libs/uuid/include
    third-party/boost-extract/libs/detail/include
    third-party/boost-extract/libs/random/include
    third-party/boost-extract/libs/integer/include
    third-party/boost-extract/libs/tti/include
    third-party/boost-extract/libs/function_types/include
    third-party/magic_enum/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(third-party/googletest)
add_subdirectory(third-party/exiv2)

include_directories(${CMAKE_BINARY_DIR})

if (LOG_LEVEL MATCHES Debug)
    add_executable(pbtests tests/testLogLevel.cpp
        tests/TestErrors.cpp
        tests/TestThirdPartyLinks.cpp
        tests/TestFilesMap.cpp)

    target_include_directories(pbtests PRIVATE include
        ${CMAKE_CURRENT_BINARY_DIR}/include
        third-party/exiv2/include
        third-party/magic_enum/include
        third-party/boost-extract
        third-party/boost-extract/libs/uuid/include
        third-party/boost-extract/libs/detail/include
        third-party/boost-extract/libs/random/include
        third-party/boost-extract/libs/integer/include
        third-party/boost-extract/libs/tti/include
        third-party/boost-extract/libs/function_types/include)

    target_link_libraries(pbtests pblib gtest gtest_main exiv2lib)
endif()