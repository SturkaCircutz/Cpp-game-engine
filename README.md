# Mini Voxel Engine

This project is a minimal C++ OpenGL voxel-style engine prototype with first-person movement.

## Features

- GLFW window and input
- System OpenGL fixed pipeline
- Mouse look
- WASD movement
- Space and Left Shift vertical movement
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
- `Esc`: quit
