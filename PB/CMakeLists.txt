cmake_minimum_required(VERSION 3.27)

project(PhotobookLib)

option(STATIC_CHECK "Statically check the program." OFF)
option(LOG_LEVEL "Log level." OFF)
option(PRINTER_TYPE "The printer type." OFF)

if (WIN32)
    add_compile_options(/std:c++20)
    add_compile_options(/EHsc)
    add_compile_options(/W4)
    add_compile_options(/wd4626)
    add_compile_options(/wd5027)
    add_compile_definitions(NOMINMAX)
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
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(third-party/podofo-build/include)
include(third-party/windows/pdfium/PDFiumConfig.cmake)

include_directories(${CMAKE_BINARY_DIR})

add_library(pblib STATIC
    include/pb/util/IteratorWithState.h
    include/pb/config/Log.h
    include/pb/config/${PRINTER_TYPE}.h
    include/pb/Command.h
    ${CMAKE_CURRENT_BINARY_DIR}/include/pb/Config.h
    include/pb/DataManager.h
    include/pb/Enums.h
    include/pb/export/ExportLogic.h
    include/pb/export/Html.h
    include/pb/export/Jpg.h
    include/pb/export/Pdf.h
    include/pb/export/PdfPoDoFo.h
    include/pb/ImageMonitor.h
    include/pb/ImportFoldersLogic.h
    include/pb/image/Image.h
    include/pb/image/ImageFactory.h
    include/pb/image/ImageOperations.h
    include/pb/image/ImageReader.h
    include/pb/image/ImageSetWriter.h
    include/pb/image/RegularImage.h
    include/pb/image/VirtualImage.h
    include/pb/image/TextImage.h
    include/pb/MetadataReader.h
    include/pb/FileComparator.h
    include/pb/persistence/FilePersistence.h
    include/pb/persistence/Persistence.h
    include/pb/persistence/SerializationStrategy.h
    include/pb/persistence/SQLPersistence.h
    include/pb/persistence/ProjectPersistence.h
    include/pb/PhotoBook.h
    include/pb/PhotobookListener.h
    include/pb/Platform.h
    include/pb/project/PaperSettings.h
    include/pb/project/PathCache.h
    include/pb/project/ProjectMetadata.h
    include/pb/project/ProjectSnapshot.h
    include/pb/project/Project.h
    include/pb/tasks/Tasks.h
    include/pb/tasks/FileMapper.h
    include/pb/tasks/ParallelTaskConsumer.h
    include/pb/tasks/SequentialTaskConsumer.h
    include/pb/tasks/ThumbnailsProcessor.h
    include/pb/StagedImages.h
    include/pb/util/Util.h
    include/pb/util/Concepts.h
    include/pb/util/Error.h
    include/pb/util/FileInfo.h
    include/pb/util/Observable.h
    include/pb/util/Timer.h
    include/pb/util/Traits.h
    include/pb/Visitor.h
    include/pb/WorkersManager.h

    src/Command.cpp
    src/Config.cpp
    src/DataManager.cpp
    src/Enums.cpp
    src/ExportLogic.cpp
    src/FileMapper.cpp
    src/FileComparator.cpp
    src/FilePersistence.cpp
    src/Html.cpp
    src/ImageFactory.cpp
    src/ImageReader.cpp
    src/ImageMonitor.cpp
    src/ImageOperations.cpp
    src/ImageSetWriter.cpp
    src/ImportFoldersLogic.cpp
    src/Jpg.cpp
    src/Log.cpp
    src/MetadataReader.cpp
    src/PaperSettings.cpp
    src/PathCache.cpp
    src/Pdf.cpp
    src/PdfPoDoFo.cpp
    src/Persistence.cpp
    src/Photobook.cpp
    src/Project.cpp
    src/ProjectSnapshot.cpp
    src/ProjectMetadata.cpp
    src/ProjectPersistence.cpp
    src/RegularImage.cpp
    src/ThumbnailsProcessor.cpp
    src/SerializationStrategy.cpp
    src/SQLPersistence.cpp
    src/StagedImages.cpp
    src/util/FileInfo.cpp
    src/util/Timer.cpp
    src/util/SequentialTaskConsumer.cpp
    src/util/ParallelTaskConsumer.cpp
    src/TextImage.cpp
    src/VirtualImage.cpp
    src/WorkersManager.cpp)

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
    unofficial::sqlite3::sqlite3
    nlohmann_json::nlohmann_json)

set_target_properties(pblib PROPERTIES LINKER_LANGUAGE CXX)

if (LOG_LEVEL MATCHES Debug)
    add_executable(pbtests 
        tests/MockListeners.h
        tests/MockListeners.cpp
        tests/TestPersistence.cpp
        tests/TestIteratorWithState.cpp
        tests/TestComparator.cpp
        tests/TestDataManager.cpp
        tests/TestErrors.cpp
        tests/TestFolderImport.cpp
        tests/TestImageMonitor.cpp
        tests/TestParallelTaskConsumer.cpp
        tests/TestPathCache.cpp
        tests/TestPersistenceProject.cpp
        tests/TestPhotobook.cpp
        tests/TestProjectPersistence.cpp
        tests/TestSequenctialTaskConsumer.cpp
        tests/TestStagedImages.cpp
        tests/TestThirdPartyLinks.cpp
        tests/TestFilesMap.cpp)

    target_include_directories(pbtests PRIVATE
        include
        ${CMAKE_CURRENT_BINARY_DIR}/include)

    target_link_libraries(pbtests
        opencv_core
        opencv_imgcodecs
        Boost::boost
        Boost::program_options
        exiv2lib
        GTest::gtest GTest::gtest_main GTest::gmock
        magic_enum::magic_enum
        dp::thread-pool
        unofficial::libharu::hpdf
        unofficial::sqlite3::sqlite3
        bcrypt
        nlohmann_json::nlohmann_json
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/podofo-build/lib/x64/Debug/podofo.lib
        pblib)

    add_custom_command(TARGET pbtests POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_directory  # which executes "cmake - E copy_if_different..."
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/podofo-build/lib/x64/Debug"      # <--this is in-file
        "${CMAKE_CURRENT_SOURCE_DIR}/../PB/build/Debug/")                 # <--this is out-file path
endif()