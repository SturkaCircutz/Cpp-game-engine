#pragma once

struct InputState
{
    bool closeRequested = false;
    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool moveUp = false;
    bool moveDown = false;
    bool removeBlock = false;
    bool placeBlock = false;
    float lookDeltaX = 0.0f;
    float lookDeltaY = 0.0f;
};
