vcpkg\vcpkg.exe install shaderc:x64-windows
vcpkg\installed\x64-windows\tools\shaderc\glslc.exe -fshader-stage=vertex PB\pb-assets\shaders\vertex.glsl -o PB\pb-assets\shaders\vertex.spv
vcpkg\installed\x64-windows\tools\shaderc\glslc.exe -fshader-stage=fragment PB\pb-assets\shaders\lut.glsl -o PB\pb-assets\shaders\lut.spv