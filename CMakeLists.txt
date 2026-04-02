cmake_minimum_required(VERSION 3.20)
project(MiniVoxelEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_executable(MiniVoxelEngine
    src/main.cpp
    src/Camera.cpp
    src/Engine.cpp
    src/Math.cpp
    src/World.cpp
)

target_include_directories(MiniVoxelEngine PRIVATE src)

target_link_libraries(MiniVoxelEngine PRIVATE glfw OpenGL::GL OpenGL::GLU)

if(MSVC)
    target_compile_options(MiniVoxelEngine PRIVATE /W4 /permissive-)
else()
    target_compile_options(MiniVoxelEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
