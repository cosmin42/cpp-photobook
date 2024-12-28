cmake -A x64 -B PB\build-x64 -S PB -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
cmake -A ARM64 -B PB\build-ARM64 -S PB -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake