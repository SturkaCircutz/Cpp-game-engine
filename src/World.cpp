#include "World.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>

#include <cmath>

void World::Render() const
{
    for (int x = -12; x <= 12; ++x)
    {
        for (int z = -12; z <= 12; ++z)
        {
            DrawCube(static_cast<float>(x), -0.5f, static_cast<float>(z), 0.5f);
        }
    }

    for (int y = 0; y < 3; ++y)
    {
        DrawCube(3.0f, static_cast<float>(y) + 0.5f, 2.0f, 0.5f);
        DrawCube(4.0f, static_cast<float>(y) + 0.5f, 2.0f, 0.5f);
        DrawCube(3.0f, static_cast<float>(y) + 0.5f, 3.0f, 0.5f);
    }

    DrawCube(-2.0f, 0.5f, -2.0f, 0.5f);
    DrawCube(-2.0f, 1.5f, -2.0f, 0.5f);
    DrawCube(-1.0f, 0.5f, -2.0f, 0.5f);
    DrawCube(0.0f, 0.5f, -2.0f, 0.5f);
    DrawCube(0.0f, 1.5f, -2.0f, 0.5f);
}

bool World::IntersectsSolid(const Vec3& minCorner, const Vec3& maxCorner) const
{
    const int minX = static_cast<int>(std::floor(minCorner.x));
    const int maxX = static_cast<int>(std::ceil(maxCorner.x)) - 1;
    const int minY = static_cast<int>(std::floor(minCorner.y));
    const int maxY = static_cast<int>(std::ceil(maxCorner.y)) - 1;
    const int minZ = static_cast<int>(std::floor(minCorner.z));
    const int maxZ = static_cast<int>(std::ceil(maxCorner.z)) - 1;

    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            for (int z = minZ; z <= maxZ; ++z)
            {
                if (IsSolidBlock(x, y, z))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool World::IsSolidBlock(int x, int y, int z) const
{
    if (y == -1 && x >= -12 && x <= 12 && z >= -12 && z <= 12)
    {
        return true;
    }

    if ((x == 3 || x == 4) && z == 2 && y >= 0 && y <= 2)
    {
        return true;
    }

    if (x == 3 && z == 3 && y >= 0 && y <= 2)
    {
        return true;
    }

    if (x == -2 && z == -2 && (y == 0 || y == 1))
    {
        return true;
    }

    if ((x == -1 || x == 0) && z == -2 && y == 0)
    {
        return true;
    }

    if (x == 0 && z == -2 && y == 1)
    {
        return true;
    }

    return false;
}

void World::DrawCube(float centerX, float centerY, float centerZ, float halfSize) const
{
    const float x0 = centerX - halfSize;
    const float x1 = centerX + halfSize;
    const float y0 = centerY - halfSize;
    const float y1 = centerY + halfSize;
    const float z0 = centerZ - halfSize;
    const float z1 = centerZ + halfSize;

    glBegin(GL_QUADS);

    glColor3f(0.55f, 0.35f, 0.18f);
    glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y0, z0);
    glVertex3f(x0, y0, z0);

    glColor3f(0.25f, 0.72f, 0.28f);
    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z1);

    glColor3f(0.42f, 0.26f, 0.12f);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x0, y1, z0);

    glColor3f(0.38f, 0.24f, 0.10f);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z0);

    glColor3f(0.34f, 0.21f, 0.09f);
    glVertex3f(x1, y0, z1);
    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y1, z1);
    glVertex3f(x1, y1, z1);

    glColor3f(0.30f, 0.18f, 0.08f);
    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y1, z0);
    glVertex3f(x0, y1, z1);

    glEnd();
}
