# FlowFinity Pathfinding for Maya

This Maya plugin allows for the optimized simulation of pathfinding, collision-avoiding agents within complex scenes.

## Building

Development dependencies:

- CMake
- Ninja
- vcpkg

Build dependencies are handled via `vcpkg`. Make sure you have it installed correctly (may already be installed with Visual Studio 17 2022). Then, to configure, run:

```sh
cmake --preset=Debug    # for a debug build
cmake --preset=Release  # for a release build
```

On Windows, you may have to run this within the "x64 Native Tools Command Prompt for VS..." for CMake to recognize the correct build toolchain.

Then, to build, run:

```sh
cmake --build ./build_debug    # for a debug build
cmake --build ./build_release  # for a release build
```

We provide the targets `flowfinity`, `flowfinityGl`, and `flowfinityMaya` for the base logic, the OpenGL test app and the Maya plugin respectively. `flowfinity` is a static library, allowing it to be embedded in other projects if desired.

The resulting `.mll` file from the `flowfinityMaya` target will be generated at `build_<type>\flowfinity_maya\bin\flowfinityMaya.mll`. This can be placed in the `plug-ins` folder of your Maya devkit or Maya itself.