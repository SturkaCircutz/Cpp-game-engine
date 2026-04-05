#include "render/Renderer.h"

#include "core/Math.h"
#include "game/Camera.h"
#include "world/World.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>

#include <cmath>

namespace
{
    constexpr float kHalfBlockSize = 0.5f;
    constexpr float kCrosshairOuterHalfSize = 7.0f;
    constexpr float kCrosshairInnerHalfSize = 4.0f;
    constexpr float kBlockEditReach = 8.0f;
    constexpr float kOutlineInflation = 0.02f;
}

void Renderer::Initialize() const
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.52f, 0.78f, 0.96f, 1.0f);
}

void Renderer::RenderFrame(int width, int height, const Camera& camera, const World& world) const
{
    const int safeHeight = height > 0 ? height : 1;
    const float aspectRatio = static_cast<float>(width) / static_cast<float>(safeHeight);

    glViewport(0, 0, width, safeHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    ApplyPerspective(aspectRatio, 70.0f, 0.1f, 300.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    const Vec3 position = camera.GetPosition();
    const Vec3 forward = camera.GetForward();
    const Vec3 target = position + forward;
    const Vec3 up(0.0f, 1.0f, 0.0f);

    const Vec3 zAxis = Normalize(position - target);
    const Vec3 xAxis = Normalize(Cross(up, zAxis));
    const Vec3 yAxis = Cross(zAxis, xAxis);

    const float viewMatrix[16] = {
        xAxis.x, yAxis.x, zAxis.x, 0.0f,
        xAxis.y, yAxis.y, zAxis.y, 0.0f,
        xAxis.z, yAxis.z, zAxis.z, 0.0f,
        -Dot(xAxis, position), -Dot(yAxis, position), -Dot(zAxis, position), 1.0f
    };

    glLoadMatrixf(viewMatrix);

    RenderWorld(world);
    RenderTargetOutline(camera, world);
    RenderCrosshair(width, safeHeight);
}

void Renderer::ApplyPerspective(float aspectRatio, float fieldOfView, float nearPlane, float farPlane) const
{
    const float top = std::tan(Radians(fieldOfView) * 0.5f) * nearPlane;
    const float bottom = -top;
    const float right = top * aspectRatio;
    const float left = -right;

    const float matrix[16] = {
        (2.0f * nearPlane) / (right - left), 0.0f, 0.0f, 0.0f,
        0.0f, (2.0f * nearPlane) / (top - bottom), 0.0f, 0.0f,
        (right + left) / (right - left), (top + bottom) / (top - bottom), -(farPlane + nearPlane) / (farPlane - nearPlane), -1.0f,
        0.0f, 0.0f, -(2.0f * farPlane * nearPlane) / (farPlane - nearPlane), 0.0f
    };

    glLoadMatrixf(matrix);
}

void Renderer::RenderWorld(const World& world) const
{
    for (const BlockCoord& block : world.GetBlocks())
    {
        RenderCube(block.x, block.y, block.z);
    }
}

void Renderer::RenderCube(int x, int y, int z) const
{
    const float centerX = static_cast<float>(x) + kHalfBlockSize;
    const float centerY = static_cast<float>(y) + kHalfBlockSize;
    const float centerZ = static_cast<float>(z) + kHalfBlockSize;

    const float x0 = centerX - kHalfBlockSize;
    const float x1 = centerX + kHalfBlockSize;
    const float y0 = centerY - kHalfBlockSize;
    const float y1 = centerY + kHalfBlockSize;
    const float z0 = centerZ - kHalfBlockSize;
    const float z1 = centerZ + kHalfBlockSize;

    glBegin(GL_QUADS);

    glColor3f(0.45f, 0.46f, 0.48f);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z1);
    glVertex3f(x0, y0, z1);

    glColor3f(0.68f, 0.69f, 0.71f);
    glVertex3f(x0, y1, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z0);
    glVertex3f(x0, y1, z0);

    glColor3f(0.52f, 0.53f, 0.55f);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y1, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y0, z0);

    glColor3f(0.49f, 0.50f, 0.52f);
    glVertex3f(x1, y0, z0);
    glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z1);
    glVertex3f(x1, y0, z1);

    glColor3f(0.56f, 0.57f, 0.59f);
    glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z1);

    glColor3f(0.47f, 0.48f, 0.50f);
    glVertex3f(x0, y0, z0);
    glVertex3f(x0, y0, z1);
    glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z0);

    glEnd();
}

void Renderer::RenderTargetOutline(const Camera& camera, const World& world) const
{
    RaycastHit hit;
    if (!world.Raycast(camera.GetPosition(), camera.GetForward(), kBlockEditReach, hit))
    {
        return;
    }

    RenderWireCube(hit.block);
}

void Renderer::RenderWireCube(const BlockCoord& block) const
{
    const float centerX = static_cast<float>(block.x) + kHalfBlockSize;
    const float centerY = static_cast<float>(block.y) + kHalfBlockSize;
    const float centerZ = static_cast<float>(block.z) + kHalfBlockSize;

    const float inflatedHalfSize = kHalfBlockSize + kOutlineInflation;
    const float x0 = centerX - inflatedHalfSize;
    const float x1 = centerX + inflatedHalfSize;
    const float y0 = centerY - inflatedHalfSize;
    const float y1 = centerY + inflatedHalfSize;
    const float z0 = centerZ - inflatedHalfSize;
    const float z1 = centerZ + inflatedHalfSize;

    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);
    glLineWidth(2.5f);
    glColor3f(1.0f, 0.95f, 0.1f);

    glBegin(GL_LINES);
    glVertex3f(x0, y0, z0); glVertex3f(x1, y0, z0);
    glVertex3f(x1, y0, z0); glVertex3f(x1, y1, z0);
    glVertex3f(x1, y1, z0); glVertex3f(x0, y1, z0);
    glVertex3f(x0, y1, z0); glVertex3f(x0, y0, z0);

    glVertex3f(x0, y0, z1); glVertex3f(x1, y0, z1);
    glVertex3f(x1, y0, z1); glVertex3f(x1, y1, z1);
    glVertex3f(x1, y1, z1); glVertex3f(x0, y1, z1);
    glVertex3f(x0, y1, z1); glVertex3f(x0, y0, z1);

    glVertex3f(x0, y0, z0); glVertex3f(x0, y0, z1);
    glVertex3f(x1, y0, z0); glVertex3f(x1, y0, z1);
    glVertex3f(x1, y1, z0); glVertex3f(x1, y1, z1);
    glVertex3f(x0, y1, z0); glVertex3f(x0, y1, z1);
    glEnd();

    glPopAttrib();
}

void Renderer::RenderCrosshair(int width, int height) const
{
    const float centerX = static_cast<float>(width) * 0.5f;
    const float centerY = static_cast<float>(height) * 0.5f;
    const float outerLeft = centerX - kCrosshairOuterHalfSize;
    const float outerRight = centerX + kCrosshairOuterHalfSize;
    const float outerTop = centerY - kCrosshairOuterHalfSize;
    const float outerBottom = centerY + kCrosshairOuterHalfSize;
    const float innerLeft = centerX - kCrosshairInnerHalfSize;
    const float innerRight = centerX + kCrosshairInnerHalfSize;
    const float innerTop = centerY - kCrosshairInnerHalfSize;
    const float innerBottom = centerY + kCrosshairInnerHalfSize;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, static_cast<double>(width), static_cast<double>(height), 0.0, -1.0, 1.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glPushAttrib(GL_ENABLE_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_TEXTURE_2D);
    glDepthMask(GL_FALSE);

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(outerLeft, outerTop);
    glVertex2f(outerRight, outerTop);
    glVertex2f(outerRight, outerBottom);
    glVertex2f(outerLeft, outerBottom);
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(innerLeft, innerTop);
    glVertex2f(innerRight, innerTop);
    glVertex2f(innerRight, innerBottom);
    glVertex2f(innerLeft, innerBottom);
    glEnd();

    glPopAttrib();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}
