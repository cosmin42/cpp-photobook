#!/bin/bash

cmake -B PB/x64-macos \
    -DCMAKE_OSX_ARCHITECTURES="x86_64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=x64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/arm64-macos \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DSYSTEM_PLATFORM="macOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=arm64-osx \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

cmake -B PB/arm64-ios \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DCMAKE_SYSTEM_NAME="iOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=arm64-ios \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=18.1 \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
