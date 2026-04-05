cmake_minimum_required(VERSION 3.20)
project(MiniVoxelEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

add_library(MiniVoxelEngineLib
    src/app/Application.cpp
    src/core/Math.cpp
    src/game/Camera.cpp
    src/input/FirstPersonController.cpp
    src/render/Renderer.cpp
    src/world/World.cpp
)

target_include_directories(MiniVoxelEngineLib PUBLIC src)

target_link_libraries(MiniVoxelEngineLib PUBLIC glfw OpenGL::GL OpenGL::GLU)

add_executable(MiniVoxelEngine
    src/main.cpp
)

target_link_libraries(MiniVoxelEngine PRIVATE MiniVoxelEngineLib)

if(WIN32)
    add_custom_command(TARGET MiniVoxelEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:MiniVoxelEngine>
            $<TARGET_FILE_DIR:MiniVoxelEngine>
        COMMAND_EXPAND_LISTS
    )
endif()

if(MSVC)
    target_compile_options(MiniVoxelEngineLib PRIVATE /W4 /permissive-)
    target_compile_options(MiniVoxelEngine PRIVATE /W4 /permissive-)
else()
    target_compile_options(MiniVoxelEngineLib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(MiniVoxelEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()
