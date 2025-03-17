![Windows App](https://github.com/cosmin42/cpp-photobook/actions/workflows/msbuild.yml/badge.svg)
![macOS App](https://github.com/cosmin42/cpp-photobook/actions/workflows/objective-c-xcode.yml/badge.svg)
![iOS App](https://github.com/cosmin42/cpp-photobook/actions/workflows/ios-build.yml/badge.svg)
![Azure deployment](https://github.com/cosmin42/cpp-photobook/actions/workflows/main_pbnoirwebapp.yml/badge.svg)

<img src="icons/pb-noir512.png" alt="icon" width="200"/>
Photobook Noir is an app that helps you create elegant, minimalist photo books with a focus on storytelling and visual impact. It offers simple tools to organize, customize, and print your favorite memories in a sleek, high-quality format.


## Screenshot
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
