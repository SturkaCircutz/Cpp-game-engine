# Mini Voxel Engine

This project is a minimal C++ OpenGL voxel-style engine prototype with first-person movement.

## Features

- GLFW window and input
- System OpenGL fixed pipeline
- Mouse look
- WASD movement
- Jump, gravity, and block collision
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
- `Space`: jump
- `Esc`: quit

## Repository workflow

- Stable branch: `master`
- Integration branch: `develop`
- Release branch example: `release/0.1.0`
- Feature branch example: `feature/player-collision`
- Chore branch example: `chore/windows-build-fix`
- Fix branch example: `fix/camera-drift`

See `CONTRIBUTING.md` and `docs/branching.md` for the intended workflow.
