# FlowFinity Pathfinding for Maya

This plugin allows...
<!-- TODO -->

## Building

Dependencies:

- SDL2
- OpenGL

This project uses CMake to generate build files. To build with Ninja and generate `compile_commands.json` for tools like Clangd:

```sh
cmake -B ./build -G "Ninja" -DCMAKE_EXPORT_COMPILE_COMMANDS=1
```