cmake_minimum_required(VERSION 3.27)

project(PhotobookLib)

option(STATIC_CHECK "Statically check the program." OFF)

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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
endif()
if (STATIC_CHECK)
    message("Static check ON")
    set(CMAKE_CXX_CLANG_TIDY "clang-tidy;-checks=*")
endif()

if (CMAKE_GENERATOR_PLATFORM STREQUAL "")
    set(PB_PLATFORM ${CMAKE_SYSTEM_NAME})
else()
    set(PB_PLATFORM ${CMAKE_GENERATOR_PLATFORM})
endif()

set(PBLIB_LIBRARY_NAME pblib-${PB_PLATFORM})
set(PBLIB_TESTS_LIBRARY_NAME pbtests-${PB_PLATFORM})

if (ANDROID)
    set(CMAKE_THREAD_LIBS_INIT "-lpthread")
    set(CMAKE_HAVE_THREADS_LIBRARY 1)
    set(CMAKE_USE_WIN32_THREADS_INIT 0)
    set(CMAKE_USE_PTHREADS_INIT 1)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    include($ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake)
endif()

find_package(Boost REQUIRED program_options)
find_package(exiv2 CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

find_package(magic_enum CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(thread-pool CONFIG REQUIRED)
find_package(unofficial-libharu CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(inja CONFIG REQUIRED)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_package(podofo CONFIG REQUIRED)
endif()

if (WIN32)
    include_directories(third-party/podofo-build/include)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/include)

include(third-party/windows/pdfium/PDFiumConfig.cmake)

include_directories(${CMAKE_BINARY_DIR})

add_library(${PBLIB_LIBRARY_NAME} STATIC
    include/pb/AspectRatio.h
    include/pb/util/IteratorWithState.h
    include/pb/CollageMakerJob.h
    include/pb/CollageLibraryAssistant.h
    include/pb/Command.h
    include/pb/Config.h
    include/pb/BasicSVGModel.h
    include/pb/CollageTemplateInfo.h
    include/pb/CollageManager.h
    include/pb/CollageThumbnailsMakerJob.h
    include/pb/DataManager.h
    include/pb/DrawingService.h
    include/pb/Enums.h
    include/pb/export/ExportLogic.h
    include/pb/export/Html.h
    include/pb/export/Jpg.h
    include/pb/export/Pdf.h
    include/pb/export/PdfPoDoFo.h
    include/pb/export/PdfLibharu.h
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
    include/pb/MapReducer.h
    include/pb/MetadataReader.h
    include/pb/FileComparator.h
    include/pb/persistence/FilePersistence.h
    include/pb/persistence/Persistence.h
    include/pb/persistence/SerializationStrategy.h
    include/pb/persistence/SQLPersistence.h
    include/pb/persistence/PersistenceService.h
    include/pb/PhotoBook.h
    include/pb/PhotobookListener.h
    include/pb/Platform.h
    include/pb/project/PaperSettings.h
    include/pb/project/ProjectMetadata.h
    include/pb/project/Project.h
    include/pb/ProgressManager.h
    include/pb/RowProcessingData.h
    include/pb/RuntimeUUID.h
    include/pb/SkiaResources.h
    include/pb/tasks/Tasks.h
    include/pb/tasks/PicturesSearchConfig.h
    include/pb/tasks/ParallelTaskConsumer.h
    include/pb/tasks/SequentialTaskConsumer.h
    include/pb/tasks/ThumbnailsProcessor.h
    include/pb/StagedImages.h
    include/pb/SVGInflater.h
    include/pb/util/Util.h
    include/pb/util/Concepts.h
    include/pb/util/Error.h
    include/pb/util/FileInfo.h
    include/pb/util/Observable.h
    include/pb/util/Timer.h
    include/pb/util/Traits.h
    include/pb/TaskCruncher.h
    include/pb/ThreadScheduler.h
    include/pb/Visitor.h
    include/pb/WorkersManager.h

    src/CollageLibraryAssistant.cpp
    src/Command.cpp
    src/CollageMakerJob.cpp
    src/CollageThumbnailsMakerJob.cpp
    src/CollageManager.cpp
    src/Config.cpp
    src/DataManager.cpp
    src/DrawingService.cpp
    src/Enums.cpp
    src/ExportLogic.cpp
    src/PicturesSearchConfig.cpp
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
    src/MetadataReader.cpp
    src/PaperSettings.cpp
    src/Pdf.cpp
    src/PdfPoDoFo.cpp
    src/PdfLibharu.cpp
    src/Persistence.cpp
    src/Photobook.cpp
    src/ProgressManager.cpp
    src/Project.cpp
    src/ProjectMetadata.cpp
    src/PersistenceService.cpp
    src/RegularImage.cpp
    src/RuntimeUUID.cpp
    src/ThumbnailsProcessor.cpp
    src/SerializationStrategy.cpp
    src/SkiaResources.cpp
    src/SQLPersistence.cpp
    src/StagedImages.cpp
    src/SVGInflater.cpp
    src/util/FileInfo.cpp
    src/util/Timer.cpp
    src/util/SequentialTaskConsumer.cpp
    src/util/ParallelTaskConsumer.cpp
    src/TaskCruncher.cpp
    src/TextImage.cpp
    src/VirtualImage.cpp
    src/WorkersManager.cpp)

target_include_directories(${PBLIB_LIBRARY_NAME} PUBLIC
    include
    third-party/windows/pdfium/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

target_link_libraries(${PBLIB_LIBRARY_NAME} PRIVATE
    opencv_core
    Boost::boost
    Boost::program_options
    Exiv2::exiv2lib
    dp::thread-pool
    unofficial::libharu::hpdf
    unofficial::sqlite3::sqlite3
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    pantor::inja)

set_target_properties(${PBLIB_LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)

add_executable(${PBLIB_TESTS_LIBRARY_NAME} 
    tests/MockListeners.h
    tests/MockListeners.cpp
    tests/TestPersistence.cpp
    tests/TestIteratorWithState.cpp
    tests/TestComparator.cpp
    tests/TestErrors.cpp
    tests/TestFolderImport.cpp
    tests/TestImageMonitor.cpp
    tests/TestParallelTaskConsumer.cpp
    tests/TestPersistenceService.cpp
    tests/TestPhotobook.cpp
    tests/TestPersistenceService.cpp
    tests/TestRuntimeUUID.cpp
    tests/TestStagedImages.cpp
    tests/TestThirdPartyLinks.cpp
    tests/TestCollageLibraryAssistant.cpp
    tests/TestDrawingService.cpp)

target_include_directories(${PBLIB_TESTS_LIBRARY_NAME} PRIVATE
    include
    ${CMAKE_CURRENT_BINARY_DIR}/include)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    set(PLATFORM_PODOFO $<IF:$<TARGET_EXISTS:podofo_shared>,podofo_shared,podofo_static>)
endif()
if (WIN32)
    set(PLATFORM_PODOFO ${CMAKE_CURRENT_SOURCE_DIR}/third-party/podofo-build/lib/${CMAKE_GENERATOR_PLATFORM}/Debug/podofo.lib)
endif()

if (WIN32)
    set(MAYBE_BCRYPT bcrypt)
    set(PLATFORM_SKIA ${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug/skia.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug/svg.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug/skshaper.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug/skunicode_core.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug/skunicode_icu.lib)
endif()

target_link_libraries(${PBLIB_TESTS_LIBRARY_NAME} PRIVATE
    opencv_core
    opencv_imgcodecs
    Boost::boost
    Boost::program_options
    Exiv2::exiv2lib
    GTest::gtest GTest::gtest_main GTest::gmock
    magic_enum::magic_enum
    dp::thread-pool
    unofficial::libharu::hpdf
    unofficial::sqlite3::sqlite3
    ${MAYBE_BCRYPT}
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    pantor::inja
    ${PLATFORM_PODOFO}
    ${PLATFORM_SKIA}
    ${PBLIB_LIBRARY_NAME})

if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_custom_command(TARGET ${PBLIB_TESTS_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/podofo-build/lib/${CMAKE_GENERATOR_PLATFORM}/Debug"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/Debug/")

    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/")

else()
    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build/")

    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/build/")
endif()

add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/Debug"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/Debug/")

add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/")
