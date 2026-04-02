#pragma once

#include "Camera.h"
#include "World.h"

struct GLFWwindow;

class Engine
{
public:
    bool Initialize();
    void Run();
    void Shutdown();

private:
    void ProcessInput(float deltaTime);
    void RenderFrame(int width, int height);
    void ApplyPerspective(float aspectRatio, float fieldOfView, float nearPlane, float farPlane) const;

    GLFWwindow* window = nullptr;
    Camera camera;
    World world;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouseSample = true;
};
