@echo off
GOTO:MAIN

:INSTALLVCPKG
    SETLOCAL enabledelayedexpansion
        git clone https://github.com/microsoft/vcpkg.git
        cd vcpkg
        git checkout 2023.08.09
        CALL bootstrap-vcpkg.bat
        vcpkg install boost-program-options:x64-windows opencv:x64-windows boost-uuid:x64-windows expat:x64-windows brotli:x64-windows inih:x64-windows magic-enum:x64-windows exiv2:x64-windows gtest:x64-windows
        vcpkg integrate install
        cd ..
    ENDLOCAL
EXIT /B 0

:MAIN
IF exist vcpkg ( echo "Vcpkg already exists, remove the folder before installing prerequisites." ) else (CALL:INSTALLVCPKG)