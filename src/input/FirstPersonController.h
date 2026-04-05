#pragma once

#include "input/InputState.h"

struct GLFWwindow;
class Camera;
class World;

class FirstPersonController
{
public:
    void Update(GLFWwindow* window, float deltaTime, Camera& camera, World& world);
    bool ShouldClose() const;

private:
    InputState SampleInput(GLFWwindow* window);
    void ApplyLook(const InputState& input, Camera& camera) const;
    void ApplyMovement(const InputState& input, float deltaTime, Camera& camera, const World& world) const;
    void ApplyBlockEditing(const InputState& input, const Camera& camera, World& world);

    double lastMouseX = 0.0;
    double lastMouseY = 0.0;
    bool firstMouseSample = true;
    bool closeRequested = false;
    bool leftMousePressedLastFrame = false;
    bool rightMousePressedLastFrame = false;
    bool removeKeyPressedLastFrame = false;
    bool placeKeyPressedLastFrame = false;
};
