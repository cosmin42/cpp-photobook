#!/bin/bash

if [ -d "vcpkg" ];
then
    echo "vcpkg directory already exists."
else
	git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    git checkout 2024.10.21
    ./bootstrap-vcpkg.sh
    ./vcpkg install boost-program-options:x64-osx boost-uuid:x64-osx boost-random:x64-osx brotli:x64-osx opencv:x64-osx magic-enum:x64-osx exiv2:x64-osx gtest:x64-osx dp-thread-pool:x64-osx libharu:x64-osx sqlite3:x64-osx nlohmann-json:x64-osx boost-bimap:x64-osx spdlog:x64-osx inja:x64-osx skia:x64-osx podofo:x64-osx glfw3:x64-osx glew:x64-osx
    ./vcpkg install boost-program-options:arm64-osx boost-uuid:arm64-osx boost-random:arm64-osx brotli:arm64-osx opencv:arm64-osx magic-enum:arm64-osx exiv2:arm64-osx gtest:arm64-osx dp-thread-pool:arm64-osx libharu:arm64-osx sqlite3:arm64-osx nlohmann-json:arm64-osx boost-bimap:arm64-osx spdlog:arm64-osx inja:arm64-osx skia:arm64-osx podofo:arm64-osx glfw3:arm64-osx glew:arm64-osx
    ./vcpkg integrate install
    cd ..
fi