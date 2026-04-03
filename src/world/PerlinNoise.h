#pragma once

#include <array>

class PerlinNoise
{
public:
    PerlinNoise();

    float Sample(float x, float y) const;

private:
    static float Fade(float value);
    static float Lerp(float a, float b, float t);
    static float Grad(int hash, float x, float y);

    std::array<int, 512> permutation{};
};
