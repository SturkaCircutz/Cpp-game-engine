#pragma once

#include "VecMath.h"

class Camera
{
public:
    Camera();

    void UpdateDirection(float deltaYaw, float deltaPitch);
    void Move(const Vec3& offset);

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
