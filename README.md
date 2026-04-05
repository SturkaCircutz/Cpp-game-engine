# Mini Voxel Engine

This project is a minimal C++ OpenGL voxel-style engine prototype with first-person movement.

## Architecture

- `src/app`: application startup, window lifecycle, and main loop orchestration
- `src/core`: shared math and foundational utilities
- `src/game`: camera and gameplay-facing scene objects
- `src/input`: input sampling and first-person control logic
- `src/render`: frame rendering and debug overlay drawing
- `src/world`: editable voxel world data and raycast queries

## Features

- GLFW window and input
- System OpenGL fixed pipeline
- Mouse look
- WASD movement
- Space and Left Shift vertical movement
- Left click block removal
- Right click block placement
- Center-screen crosshair for aiming
- Targeted block outline for edit feedback
- Simple block world similar to a Minecraft-style sandbox scene

## Requirements

- CMake 3.20 or newer
- A C++17 compiler
- GLFW installed on the system
- OpenGL and GLU available on the system

## Build

```powershell
cmake -S . -B build
cmake --build build
```

## Run

```powershell
.\build\MiniVoxelEngine.exe
```

## Controls

- `W`, `A`, `S`, `D`: move
- Mouse: look around
- `Space`: move up
- `Left Shift`: move down
- Left mouse button: remove the targeted block
- Right mouse button: place a block on the targeted face
- `Q`: remove the targeted block
- `E`: place a block on the targeted face
- `Esc`: quit

## Repository workflow

- Stable branch: `master`
- Integration branch: `develop`
- Release branch example: `release/0.1.0`
- Feature branch example: `feature/player-collision`

See `CONTRIBUTING.md` and `docs/branching.md` for the intended workflow.
