#!/bin/bash

cmake -B PB/arm64-ios \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DCMAKE_SYSTEM_NAME="iOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=arm64-ios \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=18.1 \
    -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake

make -j4 -C pb/arm64-ios/