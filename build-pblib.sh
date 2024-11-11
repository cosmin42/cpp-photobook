#!/bin/bash
cmake -B PB/build-x86_64 -DCMAKE_OSX_ARCHITECTURES="x86_64" -S PB -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=x64-osx -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/build-arm64 -DCMAKE_OSX_ARCHITECTURES="arm64" -S PB -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=arm64-osx -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/build-ios-arm64 -DCMAKE_OSX_ARCHITECTURES="arm64" -S PB -DCMAKE_BUILD_TYPE=Debug -DVCPKG_TARGET_TRIPLET=arm64-ios -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
