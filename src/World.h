#pragma once

class World
{
public:
    void Render() const;

private:
    void DrawCube(float centerX, float centerY, float centerZ, float halfSize) const;
};
