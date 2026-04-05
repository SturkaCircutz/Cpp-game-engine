#pragma once

#include "game/Camera.h"
#include "input/FirstPersonController.h"
#include "render/Renderer.h"
#include "world/World.h"

struct GLFWwindow;

class Application
{
public:
    bool Initialize();
    void Run();
    void Shutdown();

private:
    GLFWwindow* window = nullptr;
    Camera camera;
    World world;
    FirstPersonController controller;
    Renderer renderer;
};
