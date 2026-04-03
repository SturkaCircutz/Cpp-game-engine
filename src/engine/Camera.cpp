#include "engine/Camera.h"

Camera::Camera()
    : position(0.0f, 12.0f, 6.0f), yaw(-90.0f), pitch(0.0f)
{
}

void Camera::UpdateDirection(float deltaYaw, float deltaPitch)
{
    yaw += deltaYaw;
    pitch = Clamp(pitch + deltaPitch, -89.0f, 89.0f);
}

void Camera::Move(const Vec3& offset)
{
    position += offset;
}

void Camera::SetPosition(const Vec3& newPosition)
{
    position = newPosition;
}

Vec3 Camera::GetForward() const
{
    const float yawRadians = Radians(yaw);
    const float pitchRadians = Radians(pitch);

    const Vec3 forward(
        std::cos(yawRadians) * std::cos(pitchRadians),
        std::sin(pitchRadians),
        std::sin(yawRadians) * std::cos(pitchRadians)
    );

    return Normalize(forward);
}

Vec3 Camera::GetRight() const
{
    const Vec3 forward = GetForward();
    return Normalize(Cross(forward, Vec3(0.0f, 1.0f, 0.0f)));
}

Vec3 Camera::GetPosition() const
{
    return position;
}

float Camera::GetYaw() const
{
    return yaw;
}

float Camera::GetPitch() const
{
    return pitch;
}
