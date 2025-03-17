![Windows App](https://github.com/cosmin42/cpp-photobook/actions/workflows/msbuild.yml/badge.svg)
![macOS App](https://github.com/cosmin42/cpp-photobook/actions/workflows/objective-c-xcode.yml/badge.svg)
![iOS App](https://github.com/cosmin42/cpp-photobook/actions/workflows/ios-build.yml/badge.svg)
![Azure deployment](https://github.com/cosmin42/cpp-photobook/actions/workflows/main_pbnoirwebapp.yml/badge.svg)

# Photobook cpp exercise

![Icon](icons/pb-noir128.png) This project should be able to generate a pdf photobook based on a folder structure filled with photos.

![Dashboard preview](dashboard.png)

![Dashboard preview](table.png)

## Build and Run

### Windows
Install the required packages using vcpkg:
```install-prerequisites.bat```

Run ```build-pblib.bat```

Open the solution file from ```cpp-photobook\windows```


### macOS
Install the required packages using vcpkg:
```./install-prerequisites.sh```

Run ```build-pblib.sh```

## Tests

macOS: ```./pbtests``` on macOS.

Windows: Run the pbtests projects in the solution file.


## Static analysis
CMake with ```STATIC_CHECK=true```, it will use clang-tidy.


## Troubleshoot
The known to work vcpkg version: ```2024.11.16```
