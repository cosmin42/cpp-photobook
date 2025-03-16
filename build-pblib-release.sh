#!/bin/bash

cmake -B PB/Release/x64-macos \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB -DCMAKE_BUILD_TYPE=Release \
    -DVCPKG_TARGET_TRIPLET=x64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/Release/arm64-macos \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Release \
    -DVCPKG_TARGET_TRIPLET=arm64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

make -j4 -C pb/Release/x64-macos/ pblib-Darwin
make -j4 -C pb/Release/arm64-macos/ pblib-Darwin
