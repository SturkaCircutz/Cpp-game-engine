#include "world/TerrainGenerator.h"

#include <cmath>

namespace
{
    constexpr float kTerrainScale = 0.045f;
    constexpr float kTerrainBaseHeight = 9.0f;
    constexpr float kTerrainAmplitude = 10.0f;
    constexpr float kDetailScale = 0.11f;
    constexpr float kDetailAmplitude = 2.0f;
}

void TerrainGenerator::FillChunk(Chunk& chunk) const
{
    for (int localZ = 0; localZ < Chunk::kSizeZ; ++localZ)
    {
        for (int localX = 0; localX < Chunk::kSizeX; ++localX)
        {
            const int worldX = chunk.GetChunkX() * Chunk::kSizeX + localX;
            const int worldZ = chunk.GetChunkZ() * Chunk::kSizeZ + localZ;

            const float primaryNoise = noise.Sample(
                static_cast<float>(worldX) * kTerrainScale,
                static_cast<float>(worldZ) * kTerrainScale
            );

            const float detailNoise = noise.Sample(
                static_cast<float>(worldX) * kDetailScale + 200.0f,
                static_cast<float>(worldZ) * kDetailScale + 200.0f
            );

            const float terrainHeightNoise =
                primaryNoise * kTerrainAmplitude +
                detailNoise * kDetailAmplitude;

            int topY = static_cast<int>(std::floor(kTerrainBaseHeight + terrainHeightNoise));
            if (topY < 1)
            {
                topY = 1;
            }
            if (topY >= Chunk::kSizeY)
            {
                topY = Chunk::kSizeY - 1;
            }

            for (int localY = 0; localY <= topY; ++localY)
            {
                BlockType blockType = BlockType::Stone;
                if (localY == topY)
                {
                    blockType = BlockType::Grass;
                }
                else if (localY >= topY - 2)
                {
                    blockType = BlockType::Dirt;
                }

                chunk.SetBlock(localX, localY, localZ, blockType);
            }
        }
    }
}
