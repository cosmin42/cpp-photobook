@echo off
GOTO:MAIN
::TODO: Add checks for the commands, remove files that are not libs
:INSTALLVCPKG
    SETLOCAL enabledelayedexpansion
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        git checkout  2024.11.16
        CALL bootstrap-vcpkg.bat
        vcpkg install boost-program-options:arm64-windows opencv:arm64-windows boost-uuid:arm64-windows expat:arm64-windows brotli:arm64-windows inih:arm64-windows magic-enum:arm64-windows exiv2:arm64-windows gtest:arm64-windows dp-thread-pool:arm64-windows libharu:arm64-windows sqlite3:arm64-windows nlohmann-json:arm64-windows boost-bimap:arm64-windows spdlog:arm64-windows inja:arm64-windows cpp-httplib:arm64-windows
        vcpkg install boost-program-options:x64-windows opencv:x64-windows boost-uuid:x64-windows expat:x64-windows brotli:x64-windows inih:x64-windows magic-enum:x64-windows exiv2:x64-windows gtest:x64-windows dp-thread-pool:x64-windows libharu:x64-windows sqlite3:x64-windows nlohmann-json:x64-windows boost-bimap:x64-windows spdlog:x64-windows inja:x64-windows cpp-httplib:x64-windows
        vcpkg install boost-program-options:arm64-android opencv:arm64-android boost-uuid:arm64-android expat:arm64-android brotli:arm64-android inih:arm64-android magic-enum:arm64-android exiv2:arm64-android gtest:arm64-android dp-thread-pool:arm64-android libharu:arm64-android sqlite3:arm64-android nlohmann-json:arm64-android boost-bimap:arm64-android spdlog:arm64-android inja:arm64-android cpp-httplib:arm64-android
        vcpkg integrate install
        cd ..

        git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
        git clone https://skia.googlesource.com/skia.git

        python download.py https://github.com/ninja-build/ninja/releases/download/v1.12.1/ninja-win.zip
        python unzip ninja.zip .
        cd skia
        python3 tools/git-sync-deps

        ..\depot_tools\gn gen ..\PB\third-party\windows\skia\x64\Debug --args="is_official_build=true is_trivial_abi=true is_debug=false target_cpu=\"x64\" skia_enable_svg=true skia_use_vulkan=true skia_use_system_zlib=false skia_use_system_harfbuzz=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_expat=false extra_cflags=[\"/MDd\"] skia_use_system_icu=false"
        ..\depot_tools\gn gen ..\PB\third-party\windows\skia\arm64\Debug --args="is_official_build=true is_trivial_abi=true is_debug=false target_cpu=\"arm64\" skia_enable_svg=true skia_use_vulkan=true skia_use_system_zlib=false skia_use_system_harfbuzz=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_expat=false extra_cflags=[\"/MDd\"] skia_use_system_icu=false"
        ..\depot_tools\gn gen ..\PB\third-party\windows\skia\x64\Release --args="is_official_build=true is_trivial_abi=true is_debug=false target_cpu=\"x64\" skia_enable_svg=true skia_use_vulkan=true skia_use_system_zlib=false skia_use_system_harfbuzz=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_expat=false extra_cflags=[\"/MDd\"] skia_use_system_icu=false"
        ..\depot_tools\gn gen ..\PB\third-party\windows\skia\arm64\Release --args="is_official_build=true is_trivial_abi=true is_debug=false target_cpu=\"arm64\" skia_enable_svg=true skia_use_vulkan=true skia_use_system_zlib=false skia_use_system_harfbuzz=false skia_use_system_libjpeg_turbo=false skia_use_system_libpng=false skia_use_system_libwebp=false skia_use_system_expat=false extra_cflags=[\"/MDd\"] skia_use_system_icu=false"

        ..\ninja.exe -C ..\PB\third-party\windows\skia\x64\Debug
        ..\ninja.exe -C ..\PB\third-party\windows\skia\arm64\Debug
        ..\ninja.exe -C ..\PB\third-party\windows\skia\x64\Release
        ..\ninja.exe -C ..\PB\third-party\windows\skia\arm64\Release
        cd ..
    ENDLOCAL
EXIT /B 0

:MAIN
IF exist vcpkg ( echo "Vcpkg already exists, remove the folder before installing prerequisites." ) else (CALL:INSTALLVCPKG)