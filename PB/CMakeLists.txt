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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexperimental-library")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
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
if(NOT ${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    find_package(glfw3 CONFIG REQUIRED)
    find_package(GLEW REQUIRED)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_package(podofo CONFIG REQUIRED)
endif()
if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    find_package(podofo CONFIG REQUIRED)
endif()

if (WIN32)
    include_directories(third-party/podofo-build/include)
endif()

if (WIN32)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/include)
endif()

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/include)
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party/ios/skia/include)
endif()

include(third-party/windows/pdfium/PDFiumConfig.cmake)

include_directories(${CMAKE_BINARY_DIR})

add_library(${PBLIB_LIBRARY_NAME} STATIC
    include/pb/Config.h
    include/pb/Enums.h
    include/pb/ImageMonitor.h
    include/pb/PhotoBook.h
    include/pb/PhotobookListener.h
    include/pb/Platform.h
    include/pb/StagedImages.h
    include/pb/components/CollageLibraryAssistant.h
    include/pb/components/ImageMetadataInspector.h
    include/pb/components/ImageToPaperTask.h
    include/pb/components/ImportImageTask.h
    include/pb/components/MapReducer.h
    include/pb/components/OGLEngine.h
    include/pb/components/RuntimeUUID.h
    include/pb/components/SVGInflater.h
    include/pb/components/Serializer.h
    include/pb/components/SerializationStrategy.h
    include/pb/components/SkiaResources.h
    include/pb/components/TSQueue.h
    include/pb/components/TaskCruncher.h
    include/pb/components/ThreadScheduler.h
    include/pb/components/ThumbnailsTask.h
    include/pb/entities/AspectRatio.h
    include/pb/entities/BasicSVGModel.h
    include/pb/entities/CollageTemplateInfo.h
    include/pb/entities/ImageProcessingData.h
    include/pb/entities/LutIconInfo.h
    include/pb/entities/LutImageProcessingData.h
    include/pb/entities/PaperSettings.h
    include/pb/entities/ProjectMetadata.h
    include/pb/entities/RowProcessingData.h
    include/pb/entities/CollageImage.h
    include/pb/entities/GenericImage.h
    include/pb/entities/RegularImageV2.h
    include/pb/entities/TextImageV2.h
    include/pb/export/ExportLogic.h
    include/pb/export/Html.h
    include/pb/export/Jpg.h
    include/pb/export/Pdf.h
    include/pb/export/PdfLibharu.h
    include/pb/export/PdfPoDoFo.h
    include/pb/image/ImageFactory.h
    include/pb/image/ImageOperations.h
    include/pb/image/ImageReader.h
    include/pb/jobs/CollageMakerJob.h
    include/pb/jobs/CollageThumbnailsMakerJob.h
    include/pb/jobs/DirectoryInspectionJob.h
    include/pb/jobs/LutIconsPreprocessingJob.h
    include/pb/jobs/PicturesSearchJob.h
    include/pb/jobs/ThumbnailsJob.h
    include/pb/project/Project.h
    include/pb/services/CollageService.h
    include/pb/services/DatabaseService.h
    include/pb/services/DirectoryInspectionService.h
    include/pb/services/DrawingService.h
    include/pb/services/DurableHashService.h
    include/pb/services/ImageToPaperService.h
    include/pb/services/ImportFoldersService.h
    include/pb/services/LutService.h
    include/pb/services/ProgressService.h
    include/pb/services/ProjectManagementService.h
    include/pb/services/ProjectSerializerService.h
    include/pb/tasks/ParallelTaskConsumer.h
    include/pb/util/Concepts.h
    include/pb/util/Error.h
    include/pb/util/FileInfo.h
    include/pb/util/IteratorWithState.h
    include/pb/util/Observable.h
    include/pb/util/Timer.h
    include/pb/util/Traits.h

    src/CollageImage.cpp
    src/CollageLibraryAssistant.cpp
    src/CollageMakerJob.cpp
    src/CollageService.cpp
    src/CollageThumbnailsMakerJob.cpp
    src/Config.cpp
    src/DatabaseService.cpp
    src/DirectoryInspectionJob.cpp
    src/DirectoryInspectionService.cpp
    src/DrawingService.cpp
    src/DurableHashService.cpp
    src/Enums.cpp
    src/ExportLogic.cpp
    src/GenericImage.cpp
    src/Html.cpp
    src/ImageFactory.cpp
    src/ImageMetadataInspector.cpp
    src/ImageMonitor.cpp
    src/ImageOperations.cpp
    src/ImageReader.cpp
    src/ImageToPaperService.cpp
    src/ImageToPaperTask.cpp
    src/ImportFoldersService.cpp
    src/ImportImageTask.cpp
    src/Jpg.cpp
    src/LutIconsPreprocessingJob.cpp
    src/LutService.cpp
    src/OGLEngine.cpp
    src/PaperSettings.cpp
    src/Pdf.cpp
    src/PdfLibharu.cpp
    src/PdfPoDoFo.cpp
    src/Photobook.cpp
    src/PicturesSearchJob.cpp
    src/ProgressService.cpp
    src/Project.cpp
    src/ProjectManagementService.cpp
    src/ProjectMetadata.cpp
    src/ProjectSerializerService.cpp
    src/RegularImageV2.cpp
    src/RuntimeUUID.cpp
    src/SVGInflater.cpp
    src/Serializer.cpp
    src/SerializationStrategy.cpp
    src/SkiaResources.cpp
    src/StagedImages.cpp
    src/TaskCruncher.cpp
    src/TextImageV2.cpp
    src/ThumbnailsJob.cpp
    src/ThumbnailsTask.cpp
    src/util/FileInfo.cpp
    src/util/ParallelTaskConsumer.cpp
    src/util/Timer.cpp
    )

target_include_directories(${PBLIB_LIBRARY_NAME} PUBLIC
    include
    third-party/windows/pdfium/include
    ${CMAKE_CURRENT_BINARY_DIR}/include)


if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
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
else()
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
        pantor::inja
        glfw
        GLEW::GLEW)
endif()

set_target_properties(${PBLIB_LIBRARY_NAME} PROPERTIES LINKER_LANGUAGE CXX)

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set_target_properties(${PBLIB_LIBRARY_NAME} PROPERTIES
        XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
        XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ""
        XCODE_ATTRIBUTE_ENABLE_BITCODE "NO")
endif()

add_executable(${PBLIB_TESTS_LIBRARY_NAME} 
    tests/MockListeners.h
    tests/MockListeners.cpp
    tests/TestCollageService.cpp
    tests/TestIteratorWithState.cpp
    tests/TestErrors.cpp
    tests/TestFolderImport.cpp
    tests/TestImageMonitor.cpp
    tests/TestParallelTaskConsumer.cpp
    tests/TestPhotobook.cpp
    tests/TestRuntimeUUID.cpp
    tests/TestStagedImages.cpp
    tests/TestThirdPartyLinks.cpp
    tests/TestCollageLibraryAssistant.cpp
    tests/TestDrawingService.cpp
    tests/TestDatabaseService.cpp
    tests/TestLutService.cpp
    tests/TestMain.cpp
    tests/TestDurableHashService.cpp
    tests/TestProjectSerializerService.cpp
    tests/TestProjectManagementSystem.cpp
    tests/TestSerializer.cpp)

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

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(PLATFORM_SKIA ${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/${CMAKE_OSX_ARCHITECTURES}/libskia.a
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/${CMAKE_OSX_ARCHITECTURES}/libsvg.a
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/${CMAKE_OSX_ARCHITECTURES}/libskshaper.a
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/${CMAKE_OSX_ARCHITECTURES}/libskunicode_core.a
        ${CMAKE_CURRENT_SOURCE_DIR}/third-party/osx/skia/${CMAKE_OSX_ARCHITECTURES}/libskunicode_icu.a)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
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
else()
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
        ${PBLIB_LIBRARY_NAME}
        glfw
        GLEW::GLEW)
endif()

if (WIN32)
    add_custom_command(TARGET ${PBLIB_TESTS_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/podofo-build/lib/${CMAKE_GENERATOR_PLATFORM}/Debug"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/Debug/" VERBATIM)

    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_OSX_ARCHITECTURES}/")

    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_OSX_ARCHITECTURES}/")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-ios-arm64/")

    add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/build-ios-arm64/")
endif()


#add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory
#        "${CMAKE_CURRENT_SOURCE_DIR}/third-party/windows/skia/${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}"
#        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/${CMAKE_BUILD_TYPE}/" VERBATIM)

#add_custom_command(TARGET ${PBLIB_LIBRARY_NAME} POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/pb-assets"
#        "${CMAKE_CURRENT_SOURCE_DIR}/build-${CMAKE_GENERATOR_PLATFORM}/")
