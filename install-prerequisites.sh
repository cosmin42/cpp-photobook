#!/bin/bash

if [ -d "vcpkg" ];
then
    echo "vcpkg directory already exists."
else
	git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    git checkout 2024.11.16
    ./bootstrap-vcpkg.sh
    ./vcpkg install boost-program-options:x64-osx boost-uuid:x64-osx boost-random:x64-osx brotli:x64-osx opencv:x64-osx magic-enum:x64-osx exiv2:x64-osx gtest:x64-osx dp-thread-pool:x64-osx libharu:x64-osx sqlite3:x64-osx nlohmann-json:x64-osx boost-bimap:x64-osx spdlog:x64-osx inja:x64-osx podofo:x64-osx harfbuzz:x64-osx icu:x64-osx pkgconf:x64-osx
    ./vcpkg install boost-program-options:arm64-osx boost-uuid:arm64-osx boost-random:arm64-osx brotli:arm64-osx opencv:arm64-osx magic-enum:arm64-osx exiv2:arm64-osx gtest:arm64-osx dp-thread-pool:arm64-osx libharu:arm64-osx sqlite3:arm64-osx nlohmann-json:arm64-osx boost-bimap:arm64-osx spdlog:arm64-osx inja:arm64-osx podofo:arm64-osx harfbuzz:arm64-osx icu:arm64-osx pkgconf:arm64-osx
    ./vcpkg install boost-program-options:arm64-ios boost-uuid:arm64-ios boost-random:arm64-ios brotli:arm64-ios opencv:arm64-ios magic-enum:arm64-ios exiv2:arm64-ios gtest:arm64-ios dp-thread-pool:arm64-ios libharu:arm64-ios sqlite3:arm64-ios nlohmann-json:arm64-ios boost-bimap:arm64-ios spdlog:arm64-ios inja:arm64-ios podofo:arm64-ios
    ./vcpkg integrate install
    cd ..

    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
    git clone https://skia.googlesource.com/skia.git

    cd skia
    python3 tools/git-sync-deps

    bin/gn gen \
        ../PB/third-party/osx/skia/x86_64 \
        --ide=xcode \
        --args="is_official_build=true \
            is_trivial_abi=false \
            is_debug=false \
            target_cpu=\"x64\" \
            skia_enable_svg=true \
            skia_use_system_zlib=false \
            skia_use_system_harfbuzz=false \
            skia_use_system_libjpeg_turbo=false \
            skia_use_system_libpng=false \
            skia_use_system_libwebp=false \
            skia_use_system_expat=false \
            skia_use_system_icu=false \
            extra_cflags_cc=[\"-frtti\"]"

    bin/gn gen \
        ../PB/third-party/osx/skia/arm64 \
        --ide=xcode \
        --args="is_official_build=true \
            is_trivial_abi=false \
            is_debug=false \
            target_cpu=\"arm64\" \
            skia_enable_svg=true \
            skia_use_system_zlib=false \
            skia_use_system_harfbuzz=false \
            skia_use_system_libjpeg_turbo=false \
            skia_use_system_libpng=false \
            skia_use_system_libwebp=false \
            skia_use_system_expat=false \
            skia_use_system_icu=false \
            extra_cflags_cc=[\"-frtti\"]"

    bin/gn gen \
        ../PB/third-party/ios/skia/arm64 \
        --ide=xcode \
        --args="is_official_build=true \
            is_trivial_abi=false \
            is_debug=false \
            target_os=\"ios\" \
            target_cpu=\"arm64\" \
            skia_enable_svg=true \
            skia_use_system_zlib=false \
            skia_use_system_harfbuzz=false \
            skia_use_system_libjpeg_turbo=false \
            skia_use_system_libpng=false \
            skia_use_system_libwebp=false \
            skia_use_system_expat=false \
            skia_use_system_icu=false \
            extra_cflags_cc=[\"-frtti\"]"
    ninja -C ../PB/third-party/osx/skia/x86_64
    ninja -C ../PB/third-party/osx/skia/arm64
    ninja -C ../PB/third-party/ios/skia/arm64
    cd ..
fi