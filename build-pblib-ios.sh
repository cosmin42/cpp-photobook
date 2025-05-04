#!/bin/bash

cmake -B PB/Debug/arm64-ios \
    -DCMAKE_OSX_ARCHITECTURES="arm64" \
    -DCMAKE_SYSTEM_NAME="iOS" \
    -S PB \
    -DCMAKE_BUILD_TYPE=Debug \
    -DVCPKG_TARGET_TRIPLET=arm64-ios \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=18.1 \
    -DCMAKE_TOOLCHAIN_FILE=../vcpkg/scripts/buildsystems/vcpkg.cmake

make -j4 -C pb/Debug/arm64-ios/

rm -rf tmp_libvukan
mkdir tmp_libvukan
mkdir tmp_libvukan/macos
mkdir tmp_libvukan/macos-x64
mkdir tmp_libvukan/macos-arm64
mkdir tmp_libvukan/ios-arm64

cp ./vcpkg/installed/arm64-osx/debug/lib/libvulkan.dylib tmp_libvukan/macos-arm64/libvulkan.dylib
cp ./vcpkg/installed/x64-osx/debug/lib/libvulkan.dylib tmp_libvukan/macos-x64/libvulkan.dylib
cp ./vcpkg/installed/arm64-ios/debug/lib/libvulkan.dylib tmp_libvukan/ios-arm64/libvulkan.dylib

cp ./vcpkg/installed/arm64-osx/debug/lib/libvulkan.1.dylib tmp_libvukan/macos-arm64/libvulkan.1.dylib
cp ./vcpkg/installed/x64-osx/debug/lib/libvulkan.1.dylib tmp_libvukan/macos-x64/libvulkan.1.dylib
cp ./vcpkg/installed/arm64-ios/debug/lib/libvulkan.1.dylib tmp_libvukan/ios-arm64/libvulkan.1.dylib

cp ./vcpkg/installed/arm64-osx/debug/lib/libvulkan.1.3.296.dylib tmp_libvukan/macos-arm64/libvulkan.1.3.296.dylib
cp ./vcpkg/installed/x64-osx/debug/lib/libvulkan.1.3.296.dylib tmp_libvukan/macos-x64/libvulkan.1.3.296.dylib
cp ./vcpkg/installed/arm64-ios/debug/lib/libvulkan.1.3.296.dylib tmp_libvukan/ios-arm64/libvulkan.1.3.296.dylib


lipo -create \
  -output tmp_libvukan/macos/libvulkan.dylib \
  tmp_libvukan/macos-arm64/libvulkan.dylib \
  tmp_libvukan/macos-x64/libvulkan.dylib

xcodebuild -create-xcframework \
  -library tmp_libvukan/macos/libvulkan.dylib \
  -library tmp_libvukan/ios-arm64/libvulkan.dylib \
  -output tmp_libvukan/libvulkan.xcframework

lipo -create \
  -output tmp_libvukan/macos/libvulkan.1.dylib \
  tmp_libvukan/macos-arm64/libvulkan.1.dylib \
  tmp_libvukan/macos-x64/libvulkan.1.dylib

xcodebuild -create-xcframework \
  -library tmp_libvukan/macos/libvulkan.1.dylib \
  -library tmp_libvukan/ios-arm64/libvulkan.1.dylib \
  -output tmp_libvukan/libvulkan.1.xcframework

lipo -create \
  -output tmp_libvukan/macos/libvulkan.1.3.296.dylib \
  tmp_libvukan/macos-arm64/libvulkan.1.3.296.dylib \
  tmp_libvukan/macos-x64/libvulkan.1.3.296.dylib

xcodebuild -create-xcframework \
  -library tmp_libvukan/macos/libvulkan.1.3.296.dylib \
  -library tmp_libvukan/ios-arm64/libvulkan.1.3.296.dylib \
  -output tmp_libvukan/libvulkan.1.3.296.xcframework

cp -r tmp_libvukan/libvulkan.xcframework osx/PhotoBook/common-debug/
cp -r tmp_libvukan/libvulkan.1.xcframework osx/PhotoBook/common-debug/
cp -r tmp_libvukan/libvulkan.1.3.296.xcframework osx/PhotoBook/common-debug/

rm -rf tmp_libvukan