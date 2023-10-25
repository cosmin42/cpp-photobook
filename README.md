![Console App](https://github.com/cosmin42/cpp-photobook/actions/workflows/cmake-multi-platform.yml/badge.svg)
![WindowsAppSDK App](https://github.com/cosmin42/cpp-photobook/actions/workflows/msbuild.yml/badge.svg)

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

g++-13 compiler is needed.
Install the required packages using vcpkg:
```./install-prerequisites.sh```

Run ```build-pblib.sh```

Go to PB/build
Run ```make -j4```

Run ```./PhotoBook``` from the build folder.

### Console
Run ```cmake --preset pb-console-debug```

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

## Features to come

Platforms:
- [ ] Windows Desktop
- [ ] Windows Tablet
- [ ] Android Tablet
- [ ] MacOs
- [ ] iPad
- [ ] Android Phone
- [ ] iPhone

General features:
- [ ] Collages
- [ ] Low memory PDF creation
- [ ] Multiple paper size options
- [ ] HTML export
- [ ] Photo Info section
- [ ] Photos sorting
- [ ] Summary info (Places, Photo details index)
- [ ] Ability to create pages

Image layout:
 - [ ] Margins
 - [ ] Captions
 - [ ] Custom text insertion

Image editing:
 - [ ] Image background color
 - [ ] Crop
 - [ ] Rotation
 - [ ] Resize
 - [ ] Hue/Saturation
 - [ ] Contrast/Exposure
 - [ ] Filters

AI:
- [ ] Face detection and recognition
