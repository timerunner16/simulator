# Simulator
It's a robot simulator. There's robots! (once we make them.)

## Development

This codebase is developed in modern C++ (as reasonable), using the C++23
standard. The primary target is Web via Emscripten, with Linux tested as well.

## Project Structure

simulator is structured as eight different modules so as to make working in
parallel easier.

At the bottom of the system is core, depending only on external libraries; then
four lower level libraries, which also depend on core; then two higher level
libraries, depending on the lower level libraries; and game, which stitches
everything together with an entrypoint.

```
CORE:
Core <core/core.hxx>
- container types e.g. Option, Result
- size types e.g. u64, i16, f32
- geometry types e.g. vec3, mat4
- globally useful utilities

LOW LEVEL:
Render <render/render.hxx>
- portal to sokol_gfx context
- meshes
- textures
- shaders/materials
- framebuffer management

Input <input/input.hxx>
- keyboard
- mouse
- joysticks

FileAccess <fileaccess/fileaccess.hxx>
- load binary blobs
- saves
- fetch on web
- thread safe

Audio <audio/audio.hxx>
- portal to sokol_audio context
- manage the sound buffers
- provide audio objects

HIGH LEVEL:
Entity <entity/entity.hxx>
- scene structure
  - hold physics context
  - manage entity hierarchy
- visual entities
  - meshes in the scene w/o physics
- physical entities
  - physboxes in the scene w/ meshes

Resources <resources/resources.hxx>
- wrap over FileAccess
- request assets by name and type
- refcount assets to deduplicate
- async loading w/ placeholders

GAME:
Game (no header)
- entrypoint
- event loop
- all game logic
```

## Dependencies

Libraries used:
- [sokol](https://github.com/floooh/sokol) for windowing, graphics, and more.
- [DearImGui](https://github.com/ocornut/imgui) for debug immediate-mode UI
- [Lua](https://www.lua.org/) for embedded scripting
- [sol2](https://github.com/ThePhD/sol2) for a better way to use Lua

Toolchain used:
- [CMake](https://www.cmake.org/) for build system
- [Emscripten](https://emscripten.org/) for targeting Web
- [sokol-tools fork](https://github.com/timerunner16/sokol-tools) for shader build

## Build Instructions
### Setup toolchain

Install CMake, Emscripten, and the sokol-tools fork linked above. For
sokol-tools, make sure sokol-shdc is in your path.

### Prepare the build

Clone and setup the repo:

```
git clone https://github.com/Team1100/simulator
cd simulator
git submodule update --init
```

To setup web build, use emcmake:

`emcmake cmake -B build -S .`

To setup Linux build, use cmake normally:

`cmake -B build -S .`

Once the build is set up, begin building:

`cmake --build build`

### Run the app

Run a web build with emrun:

`emrun build/index.html`

Run a Linux build:

`./build/simulator`
