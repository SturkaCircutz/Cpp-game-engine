cmake_minimum_required(VERSION 3.20)
project(MiniVoxelEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(MiniVoxelEngine
    src/app/main.cpp
    src/engine/Camera.cpp
    src/engine/Engine.cpp
    src/math/VecMath.cpp
    src/render/TextureAtlas.cpp
    src/world/Chunk.cpp
    src/world/ChunkMesher.cpp
    src/world/PerlinNoise.cpp
    src/world/TerrainGenerator.cpp
    src/world/World.cpp
)

target_include_directories(MiniVoxelEngine PRIVATE src)

target_link_libraries(MiniVoxelEngine PRIVATE glfw OpenGL::GL OpenGL::GLU)

if(MSVC)
    target_compile_options(MiniVoxelEngine PRIVATE /W4 /permissive-)
else()
    target_compile_options(MiniVoxelEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
