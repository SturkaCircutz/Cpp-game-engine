#pragma once

#include "world/Chunk.h"
#include "world/PerlinNoise.h"

class TerrainGenerator
{
public:
    void FillChunk(Chunk& chunk) const;

private:
    PerlinNoise noise;
};
