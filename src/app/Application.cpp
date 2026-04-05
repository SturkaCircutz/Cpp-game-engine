#include "app/Application.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GLFW/glfw3.h>

#include <iostream>

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;
}

bool Application::Initialize()
{
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "Failed to initialize GLFW.\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Mini Voxel Engine", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed to create window.\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    renderer.Initialize();
    return true;
}

void Application::Run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        world.EnsureChunksAround(camera.GetPosition());
        controller.Update(window, deltaTime, camera, world);
        world.EnsureChunksAround(camera.GetPosition());

        if (controller.ShouldClose())
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);

        renderer.RenderFrame(width, height, camera, world);
        glfwSwapBuffers(window);
    }
}

void Application::Shutdown()
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}
