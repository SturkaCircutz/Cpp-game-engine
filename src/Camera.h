#pragma once

#include "Math.h"

class Camera
{
public:
    Camera();

    void UpdateDirection(float deltaYaw, float deltaPitch);
    void Move(const Vec3& offset);
    void SetPosition(const Vec3& newPosition);

    Vec3 GetForward() const;
    Vec3 GetRight() const;
    Vec3 GetPosition() const;
    float GetYaw() const;
    float GetPitch() const;

private:
    Vec3 position;
    float yaw;
    float pitch;
};
