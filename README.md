# FlowFinity Pathfinding for Maya

This plugin allows...
<!-- TODO -->

## Building

Dependencies:

- SDL2
- OpenGL

This project uses git submodules. To download these after cloning, run:

```sh
git submodule init && git submodule update
```

We use CMake to generate build files. To build with Ninja and generate `compile_commands.json` for tools like Clangd:

```sh
cmake -B ./build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# then
cmake --build ./build
```