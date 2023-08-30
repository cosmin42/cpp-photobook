![Multi Platform Workflow](https://github.com/cosmin42/cpp-photobook/actions/workflows/cmake-multi-platform.yml/badge.svg)

# Photobook cpp exercise

This project should be able to generate a pdf photobook based on a folder structure filled with photos.

## Build and Run

[Install vcpkg.](https://vcpkg.io/en/getting-started.html)

### Windows
Install the required packages using vcpkg:
```vcpkg install boost-program-options:x64-windows opencv:x64-windows boost-uuid:x64-windows  expat:x64-windows brotli:x64-windows inih:x64-windows magic-enum:x64-windows exiv2:x64-windows gtest:x64-windows```

Go to ```cpp-photobook\PB```

Run ```cmake -B [build directory] -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DLOG_LEVEL=Debug```

Open the solution file from ```cpp-photobook\windows\PhotoBookUI```


### macOS

```./vcpkg install boost-program-options:x64-osx opencv:x64-osx boost-uuid:x64-osx  expat:x64-osx brotli:x64-osx inih:x64-osx magic-enum:x64-osx exiv2:x64-osx gtest:x64-osx```

Go to ```cpp-photobook```

```cmake -B [build directory] -S . -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake -DLOG_LEVEL=Debug```

```cmake --build [build directory]```

Run ```./PhotoBook``` from the build folder.



## Tests
The tests are generated only when ```LOG_LEVEL=Debug```

macOS: ```./pbtests``` on macOS.

Windows: Run the pbtests projects in the solution file.


## Static analysis
CMake with ```STATIC_CHECK=true```, it will use clang-tidy.


## Troubleshoot
The known to work vcpkg version: 2023.08.09 
The known to work packages versions:

## Notes
The vcpkg-export folder contains the release version of the lbraries in order to be able to test the build on Github without using git lfs.
In order to get the release version modify the x64-windows triplet adding ```set(VCPKG_BUILD_TYPE release)```