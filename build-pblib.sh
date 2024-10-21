#!/bin/bash
cmake -B PB/build -S PB -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake
