#!/bin/bash

cmake -B PB/build -S PB -DCMAKE_C_COMPILER=/usr/local/bin/clang -DCMAKE_CXX_COMPILER=/usr/local/bin/clang++  -DCMAKE_CXX_FLAGS=`-isysroot $(xcrun --show-sdk-path)` -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake