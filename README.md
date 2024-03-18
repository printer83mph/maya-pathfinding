# FlowFinity Pathfinding for Maya

This plugin allows...
<!-- TODO -->

## Building

Dependencies:

- SDL2
- GLEW

This project uses git submodules. They will be automatically fetched by CMake when configuring.

We use CMake to generate build files. To make a debug build with Ninja and generate `compile_commands.json` for tools like Clangd:

```sh
cmake -B ./build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug
```

## Known Issues

On Windows, something breaks when using Mingw64 and GCC to build, making our OpenGL engine render inconsistently. Building with Clang makes this work consistently for some reason.
