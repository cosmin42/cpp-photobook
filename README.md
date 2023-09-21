![Multi Platform Workflow](https://github.com/cosmin42/cpp-photobook/actions/workflows/cmake-multi-platform.yml/badge.svg)

# Photobook cpp exercise

This project should be able to generate a pdf photobook based on a folder structure filled with photos.

## Build and Run

[Install vcpkg.](https://vcpkg.io/en/getting-started.html)

### Windows
Install the required packages using vcpkg:
```install-prerequisites.bat```

Run ```build-pblib.bat```

Open the solution file from ```cpp-photobook\windows\PhotoBookUI```


### macOS

The default macOS clang compiler does not support the latest c++ features so llvm is needed.

```brew install llvm```

Add LLVM to path:
```export PATH=/usr/local/Cellar/llvm/16.0.6/bin:$PATH```

```./vcpkg install boost-program-options:x64-osx opencv:x64-osx boost-uuid:x64-osx  expat:x64-osx brotli:x64-osx inih:x64-osx magic-enum:x64-osx exiv2:x64-osx gtest:x64-osx```

Go to ```cpp-photobook```

```
cmake -B [build directory] -S . \
-DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake \
-DLOG_LEVEL=Debug \
-DCMAKE_C_COMPILER=clang \
-DCMAKE_CXX_COMPILER=clang++
```

```cmake --build [build directory]```

Run ```./PhotoBook``` from the build folder.



## Tests
The tests are generated only when ```LOG_LEVEL=Debug```

macOS: ```./pbtests``` on macOS.

Windows: Run the pbtests projects in the solution file.


## Static analysis
CMake with ```STATIC_CHECK=true```, it will use clang-tidy.


## Troubleshoot
The known to work vcpkg version: ```2023.08.09``` 

The known to work packages versions:
```
boost-program-options:x64-windows                 1.82.0#2
exiv2:x64-windows                                 0.27.6#3
opencv:x64-windows                                4.8.0
brotli:x64-windows                                1.0.9#5
expat:x64-windows                                 2.5.0#3
inih:x64-windows                                  57
boost-uuid:x64-windows                            1.82.0#2
magic-enum:x64-windows                            0.9.3
magic-enum:x64-windows                            0.9.3
```

## Notes
The vcpkg-export folder contains the release version of the lbraries. This way the Github is able to build the project without having to use git lfs.

In order to install the libraries release version only modify the x64-windows triplet adding ```set(VCPKG_BUILD_TYPE release)```

#### Exporting vcpkg packages
```vcpkg export --raw --x-all-installed```