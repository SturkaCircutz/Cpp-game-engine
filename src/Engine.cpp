#include "Engine.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GLFW/glfw3.h>
#include <GL/gl.h>

#include <cmath>
#include <iostream>

namespace
{
    constexpr int kWindowWidth = 1280;
    constexpr int kWindowHeight = 720;
    constexpr float kMouseSensitivity = 0.1f;
    constexpr float kMoveSpeed = 5.0f;
    constexpr float kGravity = 24.0f;
    constexpr float kJumpSpeed = 8.5f;
    constexpr float kPlayerRadius = 0.30f;
    constexpr float kPlayerHeight = 1.80f;
    constexpr float kEyeOffset = 1.62f;
}

bool Engine::Initialize()
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

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.52f, 0.78f, 0.96f, 1.0f);

    return true;
}

void Engine::Run()
{
    double lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;

        ProcessInput(deltaTime);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        RenderFrame(width, height);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Engine::Shutdown()
{
    if (window != nullptr)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

void Engine::ProcessInput(float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouseSample)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouseSample = false;
    }

    const float deltaX = static_cast<float>(mouseX - lastMouseX);
    const float deltaY = static_cast<float>(lastMouseY - mouseY);
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    camera.UpdateDirection(deltaX * kMouseSensitivity, deltaY * kMouseSensitivity);

    Vec3 forward = camera.GetForward();
    forward.y = 0.0f;
    forward = Normalize(forward);

    const Vec3 right = camera.GetRight();

    Vec3 movement;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        movement += forward;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        movement -= forward;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        movement -= right;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        movement += right;
    }

    if (Length(movement) > 0.0f)
    {
        movement = Normalize(movement) * kMoveSpeed;
    }

    const bool jumpPressed = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
    if (jumpPressed && !jumpPressedLastFrame && isGrounded)
    {
        playerVelocity.y = kJumpSpeed;
        isGrounded = false;
    }
    jumpPressedLastFrame = jumpPressed;

    playerVelocity.x = movement.x;
    playerVelocity.z = movement.z;
    playerVelocity.y -= kGravity * deltaTime;

    MovePlayer(playerVelocity * deltaTime);
}

void Engine::MovePlayer(const Vec3& velocityStep)
{
    Vec3 position = camera.GetPosition();

    position.x += velocityStep.x;
    if (IsPlayerCollidingAt(position))
    {
        position.x -= velocityStep.x;
    }

    position.y += velocityStep.y;
    if (IsPlayerCollidingAt(position))
    {
        position.y -= velocityStep.y;
        if (velocityStep.y < 0.0f)
        {
            isGrounded = true;
        }
        playerVelocity.y = 0.0f;
    }
    else
    {
        isGrounded = false;
    }

    position.z += velocityStep.z;
    if (IsPlayerCollidingAt(position))
    {
        position.z -= velocityStep.z;
    }

    camera.SetPosition(position);
}

bool Engine::IsPlayerCollidingAt(const Vec3& cameraPosition) const
{
    const Vec3 minCorner(
        cameraPosition.x - kPlayerRadius,
        cameraPosition.y - kEyeOffset,
        cameraPosition.z - kPlayerRadius
    );
    const Vec3 maxCorner(
        cameraPosition.x + kPlayerRadius,
        cameraPosition.y - kEyeOffset + kPlayerHeight,
        cameraPosition.z + kPlayerRadius
    );

    return world.IntersectsSolid(minCorner, maxCorner);
}

void Engine::RenderFrame(int width, int height)
{
    const int safeHeight = height > 0 ? height : 1;
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(safeHeight);

    glViewport(0, 0, width, safeHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ApplyPerspective(aspectRatio, 70.0f, 0.1f, 300.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const Vec3 position = camera.GetPosition();
    const Vec3 forward = camera.GetForward();
    const Vec3 target = position + forward;
    const Vec3 up(0.0f, 1.0f, 0.0f);

    const Vec3 zAxis = Normalize(position - target);
    const Vec3 xAxis = Normalize(Cross(up, zAxis));
    const Vec3 yAxis = Cross(zAxis, xAxis);

    const float viewMatrix[16] = {
        xAxis.x, yAxis.x, zAxis.x, 0.0f,
        xAxis.y, yAxis.y, zAxis.y, 0.0f,
        xAxis.z, yAxis.z, zAxis.z, 0.0f,
        -Dot(xAxis, position), -Dot(yAxis, position), -Dot(zAxis, position), 1.0f
    };

    glLoadMatrixf(viewMatrix);

    world.Render();
}

void Engine::ApplyPerspective(float aspectRatio, float fieldOfView, float nearPlane, float farPlane) const
{
    const float top = std::tan(Radians(fieldOfView) * 0.5f) * nearPlane;
    const float bottom = -top;
    const float right = top * aspectRatio;
    const float left = -right;

    const float matrix[16] = {
        (2.0f * nearPlane) / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, (2.0f * nearPlane) / (top - bottom), 0.0f, 0.0f,
        (right + left) / (right - left), (top + bottom) / (top - bottom), -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
        0.0f, 0.0f, -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane), 0.0f
    };

    glLoadMatrixf(matrix);
}
