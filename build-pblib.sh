#!/bin/bash

cmake -B PB/Debug/x64-macos \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=x64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/Debug/arm64-macos \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=arm64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

make -j4 -C pb/Debug/x64-macos/ pblib-Darwin
make -j4 -C pb/Debug/arm64-macos/ pblib-Darwin
