#include "input/FirstPersonController.h"

#include "game/Camera.h"
#include "world/World.h"

#include <algorithm>
#include <GLFW/glfw3.h>

namespace
{
    constexpr float kMouseSensitivity = 0.1f;
    constexpr float kMoveSpeed = 5.0f;
    constexpr float kBlockEditReach = 8.0f;
}

void FirstPersonController::Update(GLFWwindow* window, float deltaTime, Camera& camera, World& world)
{
    const InputState input = SampleInput(window);

    closeRequested = input.closeRequested;
    ApplyLook(input, camera);
    ApplyMovement(input, deltaTime, camera, world);
    ApplyBlockEditing(input, camera, world);
}

bool FirstPersonController::ShouldClose() const
{
    return closeRequested;
}

InputState FirstPersonController::SampleInput(GLFWwindow* window)
{
    InputState input;
    input.closeRequested = glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS;
    input.moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
    input.moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
    input.moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
    input.moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
    input.moveUp =
        glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS;
    input.moveDown =
        glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS;

    const bool leftMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    const bool rightMousePressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
    const bool removeKeyPressed = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
    const bool placeKeyPressed = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

    input.removeBlock = (leftMousePressed && !leftMousePressedLastFrame) || (removeKeyPressed && !removeKeyPressedLastFrame);
    input.placeBlock = (rightMousePressed && !rightMousePressedLastFrame) || (placeKeyPressed && !placeKeyPressedLastFrame);

    leftMousePressedLastFrame = leftMousePressed;
    rightMousePressedLastFrame = rightMousePressed;
    removeKeyPressedLastFrame = removeKeyPressed;
    placeKeyPressedLastFrame = placeKeyPressed;

    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    if (firstMouseSample)
    {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouseSample = false;
    }

    input.lookDeltaX = static_cast<float>(mouseX - lastMouseX) * kMouseSensitivity;
    input.lookDeltaY = static_cast<float>(lastMouseY - mouseY) * kMouseSensitivity;
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    return input;
}

void FirstPersonController::ApplyLook(const InputState& input, Camera& camera) const
{
    camera.UpdateDirection(input.lookDeltaX, input.lookDeltaY);
}

void FirstPersonController::ApplyMovement(const InputState& input, float deltaTime, Camera& camera, const World& world) const
{
    Vec3 forward = camera.GetForward();
    forward.y = 0.0f;
    forward = Normalize(forward);

    const Vec3 right = camera.GetRight();

    Vec3 movement;
    if (input.moveForward)
    {
        movement += forward;
    }
    if (input.moveBackward)
    {
        movement -= forward;
    }
    if (input.moveLeft)
    {
        movement -= right;
    }
    if (input.moveRight)
    {
        movement += right;
    }
    if (input.moveUp)
    {
        movement.y += 1.0f;
    }
    if (input.moveDown)
    {
        movement.y -= 1.0f;
    }

    if (Length(movement) > 0.0f)
    {
        Vec3 newPosition = camera.GetPosition() + Normalize(movement) * (kMoveSpeed * deltaTime);
        newPosition.y = std::clamp(newPosition.y, world.GetMinCameraHeight(), world.GetMaxCameraHeight());
        camera.SetPosition(newPosition);
    }
}

void FirstPersonController::ApplyBlockEditing(const InputState& input, const Camera& camera, World& world)
{
    if (!input.removeBlock && !input.placeBlock)
    {
        return;
    }

    RaycastHit hit;
    if (!world.Raycast(camera.GetPosition(), camera.GetForward(), kBlockEditReach, hit))
    {
        return;
    }

    if (input.removeBlock)
    {
        world.RemoveBlock(hit.block);
    }

    if (input.placeBlock)
    {
        world.AddBlock(hit.adjacent);
    }
}
