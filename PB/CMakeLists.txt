cmake_minimum_required(VERSION 3.27)

if (WIN32)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
else()
    set(CMAKE_CXX_FLAGS "-std=c++20")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

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
    configure_file(Config.h.in include/pb/Config.h)
else()
    message(FATAL_ERROR "${LOG_LEVEL} Not in the list of levels ${LOG_LEVEL_STRING_LIST}" )
endif()

if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

project(pblib)

find_package(OpenCV CONFIG REQUIRED)
find_package(Boost REQUIRED program_options)

add_subdirectory(third-party/magic_enum)

add_library(pblib STATIC
    include/pb/common/Log.h
    include/pb/common/${PRINTER_TYPE}.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/pb/Config.h
    include/pb/Error.h
    include/pb/Enums.h
    include/pb/FileMapper.h
    include/pb/GradualControllable.h
    include/pb/ImageReader.h
    include/pb/ImageSetWriter.h
    include/pb/MetadataReader.h
    include/pb/FileComparator.h
    include/pb/PhotoBook.h
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
    src/MetadataReader.cpp
    src/PhotoBook.cpp
    src/util/FileInfo.cpp
    src/util/Timer.cpp
    src/util/Thread.cpp
)

target_include_directories(pblib PUBLIC include
    third-party/magic_enum/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(pblib PUBLIC opencv_core Boost::boost Boost::program_options)
set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(third-party/googletest)
add_subdirectory(third-party/exiv2)

include_directories(${CMAKE_BINARY_DIR})

if (LOG_LEVEL MATCHES Debug)
    add_executable(pbtests 
        tests/TestComparator.cpp
        tests/TestErrors.cpp
        tests/TestThirdPartyLinks.cpp
        tests/TestFilesMap.cpp)

    target_include_directories(pbtests PRIVATE include
        ${CMAKE_CURRENT_BINARY_DIR}/include
        third-party/exiv2/include
        third-party/magic_enum/include)

    target_link_libraries(pbtests pblib gtest gtest_main exiv2lib)
endif()