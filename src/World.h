#pragma once

#include "VecMath.h"

class World
{
public:
    void Render() const;
    bool IntersectsSolid(const Vec3& minCorner, const Vec3& maxCorner) const;

private:
    bool IsSolidBlock(int x, int y, int z) const;
    void DrawCube(float centerX, float centerY, float centerZ, float halfSize) const;
};
