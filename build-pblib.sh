#!/bin/bash

CXX=clang++ CC=clang cmake -B PB/build -S PB -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake -DLOG_LEVEL=Debug -DPRINTER_TYPE=StandardPrinter