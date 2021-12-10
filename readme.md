# meshL - obj mesh loader

load mesh from `.obj` file, and resolve materials in `.mtl`

## Build

It's a CMake project, it should be able to build on various platforms & IDEs.

I've tested building this on:

* VS2022/2019
* MinGW-w64 gcc 11.2.0 ucrt (from MSYS2)
* Clang 13.0.0 (both Windows and Arch Linux)

### Prerequisites

* CMake 3.21 or above (for `$<TARGET_RUNTIME_DLLS:test_obj>` generator to resolve dlls on windows)
* vcpkg (for Visual Studio) or MSYS2 (for mingw-w64) is prefered to install dependencies on windows

### Dependencies

* OpenCV
* freeglut (and opengl of course)

