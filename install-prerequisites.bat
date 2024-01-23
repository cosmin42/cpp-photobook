@echo off
GOTO:MAIN

:INSTALLVCPKG
    SETLOCAL enabledelayedexpansion
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        git checkout 2023.08.09
        CALL bootstrap-vcpkg.bat
        vcpkg install boost-program-options:x64-windows opencv:x64-windows boost-uuid:x64-windows expat:x64-windows brotli:x64-windows inih:x64-windows magic-enum:x64-windows exiv2:x64-windows gtest:x64-windows dp-thread-pool:x64-windows libharu:x64-windows sqlite3:x64-windows nlohmann-json:x64-windows boost-bimap:x64-windows
        vcpkg install boost-program-options:arm64-windows opencv:arm64-windows boost-uuid:arm64-windows expat:arm64-windows brotli:arm64-windows inih:arm64-windows magic-enum:arm64-windows exiv2:arm64-windows gtest:arm64-windows dp-thread-pool:arm64-windows libharu:arm64-windows sqlite3:arm64-windows nlohmann-json:arm64-windows boost-bimap:arm64-windows
        vcpkg integrate install
        cd ..
    ENDLOCAL
EXIT /B 0

:MAIN
IF exist vcpkg ( echo "Vcpkg already exists, remove the folder before installing prerequisites." ) else (CALL:INSTALLVCPKG)