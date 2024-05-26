@echo off
GOTO:MAIN

:INSTALLVCPKG
    SETLOCAL enabledelayedexpansion
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        git checkout 2023.08.09
        CALL bootstrap-vcpkg.bat
        vcpkg install boost-program-options:x64-windows opencv:x64-windows boost-uuid:x64-windows expat:x64-windows brotli:x64-windows inih:x64-windows magic-enum:x64-windows exiv2:x64-windows gtest:x64-windows dp-thread-pool:x64-windows libharu:x64-windows sqlite3:x64-windows nlohmann-json:x64-windows boost-bimap:x64-windows spdlog:x64-windows inja:x64-windows
        vcpkg install boost-program-options:arm64-windows opencv:arm64-windows boost-uuid:arm64-windows expat:arm64-windows brotli:arm64-windows inih:arm64-windows magic-enum:arm64-windows exiv2:arm64-windows gtest:arm64-windows dp-thread-pool:arm64-windows libharu:arm64-windows sqlite3:arm64-windows nlohmann-json:arm64-windows boost-bimap:arm64-windows spdlog:arm64-windows inja:x64-windows
        vcpkg install boost-program-options:arm64-android opencv:arm64-android boost-uuid:arm64-android expat:arm64-android brotli:arm64-android inih:arm64-android magic-enum:arm64-android exiv2:arm64-android gtest:arm64-android dp-thread-pool:arm64-android libharu:arm64-android sqlite3:arm64-android nlohmann-json:arm64-android boost-bimap:arm64-android spdlog:arm64-android inja:arm64-android
        vcpkg install boost-program-options:x64-android opencv:x64-android boost-uuid:x64-android expat:x64-android brotli:x64-android inih:x64-android magic-enum:x64-android exiv2:x64-android gtest:x64-android dp-thread-pool:x64-android libharu:x64-android sqlite3:x64-android nlohmann-json:x64-android boost-bimap:x64-android spdlog:x64-android inja:x64-android
        vcpkg integrate install
        cd ..
    ENDLOCAL
EXIT /B 0

:MAIN
IF exist vcpkg ( echo "Vcpkg already exists, remove the folder before installing prerequisites." ) else (CALL:INSTALLVCPKG)