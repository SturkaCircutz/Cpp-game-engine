#pragma once

class Camera;
class World;
struct BlockCoord;

class Renderer
{
public:
    void Initialize() const;
    void RenderFrame(int width, int height, const Camera& camera, const World& world) const;

private:
    void ApplyPerspective(float aspectRatio, float fieldOfView, float nearPlane, float farPlane) const;
    void RenderWorld(const World& world) const;
    void RenderCube(int x, int y, int z) const;
    void RenderTargetOutline(const Camera& camera, const World& world) const;
    void RenderWireCube(const BlockCoord& block) const;
    void RenderCrosshair(int width, int height) const;
};
