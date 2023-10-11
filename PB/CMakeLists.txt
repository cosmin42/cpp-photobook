cmake_minimum_required(VERSION 3.27)

project(PhotoBookLib)

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)
option(PRINTER_TYPE "The printer type." OFF)

if (WIN32)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
    add_compile_options(/W4)
    add_compile_options(/wd4626)
    add_compile_options(/wd5027)
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
find_package(thread-pool CONFIG REQUIRED)
find_package(unofficial-libharu CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)


include(third-party/windows/pdfium/PDFiumConfig.cmake)

include_directories(${CMAKE_BINARY_DIR})

add_library(pblib STATIC
    include/pb/util/CircularIterator.h
    include/pb/common/Log.h
    include/pb/common/${PRINTER_TYPE}.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/pb/Config.h
    include/pb/DataManager.h
    include/pb/Error.h
    include/pb/Enums.h
    include/pb/Exporter.h
    include/pb/FileMapper.h
    include/pb/ImageReader.h
    include/pb/ImageOperations.h
    include/pb/ImageSetWriter.h
    include/pb/MediaMap.h
    include/pb/MediaMapListener.h
    include/pb/MetadataReader.h
    include/pb/FileComparator.h
    include/pb/Gallery.h
    include/pb/Pdf.h
    include/pb/persistence/Persistence.h
    include/pb/PhotoBook.h
    include/pb/Project.h
    include/pb/ThumbnailsProcessor.h
    include/pb/StagedImagesLogic.h
    include/pb/util/Concepts.h
    include/pb/util/FileInfo.h
    include/pb/util/Timer.h
    include/pb/util/Thread.h
    include/pb/util/Traits.h

    src/Config.cpp
    src/DataManager.cpp
    src/FileMapper.cpp
    src/FileComparator.cpp
    src/ImageReader.cpp
    src/ImageOperations.cpp
    src/ImageSetWriter.cpp
    src/Log.cpp
    src/MediaMapListener.cpp
    src/MetadataReader.cpp
    src/Pdf.cpp
    src/PhotoBook.cpp
    src/Project.cpp
    src/ThumbnailsProcessor.cpp
    src/StagedImagesLogic.cpp
    src/util/FileInfo.cpp
    src/util/Timer.cpp
    src/util/Thread.cpp)

target_include_directories(pblib PUBLIC
    include
    third-party/windows/pdfium/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(pblib PUBLIC
    opencv_core
    Boost::boost
    Boost::program_options
    exiv2lib
    dp::thread-pool
    unofficial::libharu::hpdf
    unofficial::sqlite3::sqlite3)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

if (LOG_LEVEL MATCHES Debug)
    add_executable(pbtests 
        tests/TestPersistence.cpp
        tests/TestCircularIterator.cpp
        tests/TestComparator.cpp
        tests/TestErrors.cpp
        tests/TestGallery.cpp
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