#!/bin/bash

if [ -d "vcpkg" ];
then
    echo "vcpkg directory already exists."
else
	git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    git checkout 2023.08.09
    ./bootstrap-vcpkg.sh
    ./vcpkg install boost-program-options:x64-osx boost-uuid:x64-osx brotli:x64-osx opencv:x64-osx magic-enum:x64-osx exiv2:x64-osx gtest:x64-osx dp-thread-pool:x64-osx libharu:x64-osx sqlite3:x64-osx nlohmann-json:x64-osx boost-bimap:x64-osx spdlog:x64-osx inja:x64-osx skia:x64-osx podofo:x64-osx glfw3:x64-osx glew:x64-osx
    ./vcpkg integrate install
    cd ..
fi