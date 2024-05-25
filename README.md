![Console App](https://github.com/cosmin42/cpp-photobook/actions/workflows/cmake-multi-platform.yml/badge.svg)
![WindowsAppSDK App](https://github.com/cosmin42/cpp-photobook/actions/workflows/msbuild.yml/badge.svg)

# Photobook cpp exercise

This project should be able to generate a pdf photobook based on a folder structure filled with photos.

## Build and Run


### Windows
Install the required packages using vcpkg:
```install-prerequisites.bat```

Run ```build-pblib.bat```

Open the solution file from ```cpp-photobook\windows```


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

macOS: ```./pbtests``` on macOS.

Windows: Run the pbtests projects in the solution file.


## Static analysis
CMake with ```STATIC_CHECK=true```, it will use clang-tidy.


## Troubleshoot
The known to work vcpkg version: ```2023.08.09``` 
