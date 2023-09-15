cmake_minimum_required(VERSION 3.27)

project(PhotoBookLib)

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)
option(PRINTER_TYPE "The printer type." OFF)

if (WIN32)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
else()
    set(CMAKE_CXX_FLAGS "-std=c++20")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# The default printer
if (NOT PRINTER_TYPE)
    set(PRINTER_TYPE StandardPrinter)
endif()

# The default log level
if (NOT LOG_LEVEL)
    set(LOG_LEVEL Debug)
endif()

set(EXISTING_LOG_LEVELS None Debug Info Warning Error)

if (LOG_LEVEL IN_LIST EXISTING_LOG_LEVELS)
    message("Generating config files...")
    configure_file(Config.h.in include/pb/Config.h)
else()
    message(FATAL_ERROR "${LOG_LEVEL} Not in the list of levels ${EXISTING_LOG_LEVELS}" )
endif()

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

find_package(Boost REQUIRED program_options)
find_package(exiv2 CONFIG REQUIRED)
if (LOG_LEVEL MATCHES Debug)
    find_package(GTest CONFIG REQUIRED)
endif()
find_package(magic_enum CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)

include_directories(${CMAKE_BINARY_DIR})

add_library(pblib STATIC
    include/pb/CircularIterator.h
    include/pb/common/Log.h
    include/pb/common/${PRINTER_TYPE}.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/pb/Config.h
    include/pb/DataManager.h
    include/pb/Error.h
    include/pb/Enums.h
    include/pb/FileMapper.h
    include/pb/GalleryListener.h
    include/pb/GradualControllable.h
    include/pb/ImageReader.h
    include/pb/ImageSetWriter.h
    include/pb/MediaMap.h
    include/pb/MediaMapListener.h
    include/pb/MetadataReader.h
    include/pb/FileComparator.h
    include/pb/Gallery.h
    include/pb/PhotoBook.h
    include/pb/Scheduable.h
    include/pb/util/Concepts.h
    include/pb/util/FileInfo.h
    include/pb/util/Timer.h
    include/pb/util/Thread.h
    include/pb/util/Traits.h

    src/Config.cpp
    src/FileMapper.cpp
    src/FileComparator.cpp
    src/GradualControllable.cpp
    src/ImageReader.cpp
    src/ImageSetWriter.cpp
    src/Log.cpp
    src/MediaMapListener.cpp
    src/MetadataReader.cpp
    src/PhotoBook.cpp
    src/util/FileInfo.cpp
    src/util/Timer.cpp
    src/util/Thread.cpp)

target_include_directories(pblib PUBLIC
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(pblib PUBLIC
    opencv_core
    Boost::boost
    Boost::program_options
    exiv2lib)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

if (LOG_LEVEL MATCHES Debug)
    add_executable(pbtests 
        tests/TestComparator.cpp
        tests/TestErrors.cpp
        tests/TestThirdPartyLinks.cpp
        tests/TestFilesMap.cpp)

    target_include_directories(pbtests PRIVATE
        include
        ${CMAKE_CURRENT_BINARY_DIR}/include)

    target_link_libraries(pbtests
        Boost::boost
        Boost::program_options
        exiv2lib
        GTest::gtest GTest::gtest_main
        magic_enum::magic_enum
        pblib)
endif()