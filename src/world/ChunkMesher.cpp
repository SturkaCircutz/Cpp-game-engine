#include "world/ChunkMesher.h"

#include "world/World.h"

void ChunkMesher::BuildChunkMesh(Chunk& chunk, const World& world) const
{
    chunk.ClearMesh();

    for (int localY = 0; localY < Chunk::kSizeY; ++localY)
    {
        for (int localZ = 0; localZ < Chunk::kSizeZ; ++localZ)
        {
            for (int localX = 0; localX < Chunk::kSizeX; ++localX)
            {
                const BlockType blockType = chunk.GetBlock(localX, localY, localZ);
                if (blockType == BlockType::Air)
                {
                    continue;
                }

                const int worldX = chunk.GetChunkX() * Chunk::kSizeX + localX;
                const int worldY = localY;
                const int worldZ = chunk.GetChunkZ() * Chunk::kSizeZ + localZ;

                const bool isStone = blockType == BlockType::Stone;
                const float sideColor[3] = {
                    isStone ? 0.45f : 0.42f,
                    isStone ? 0.45f : 0.26f,
                    isStone ? 0.48f : 0.12f
                };
                const float grassTopUvMin[2] = {0.0f, 0.0f};
                const float grassTopUvMax[2] = {0.5f, 0.5f};
                const float grassSideUvMin[2] = {0.5f, 0.0f};
                const float grassSideUvMax[2] = {1.0f, 0.5f};
                const float dirtUvMin[2] = {0.0f, 0.5f};
                const float dirtUvMax[2] = {0.5f, 1.0f};
                const float stoneUvMin[2] = {0.5f, 0.5f};
                const float stoneUvMax[2] = {1.0f, 1.0f};

                if (world.IsFaceVisible(worldX, worldY, worldZ, 0, -1, 0))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ),
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ)
                    };
                    const float color[3] = {0.32f, 0.24f, 0.14f};
                    AppendFace(chunk, dirtUvMin, dirtUvMax, color, vertices);
                }

                if (world.IsFaceVisible(worldX, worldY, worldZ, 0, 1, 0))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1)
                    };

                    float color[3] = {0.55f, 0.55f, 0.60f};
                    if (blockType == BlockType::Grass)
                    {
                        color[0] = 0.24f;
                        color[1] = 0.70f;
                        color[2] = 0.27f;
                    }
                    else if (blockType == BlockType::Dirt)
                    {
                        color[0] = 0.52f;
                        color[1] = 0.34f;
                        color[2] = 0.18f;
                    }

                    const float* uvMin = blockType == BlockType::Grass ? grassTopUvMin : (blockType == BlockType::Dirt ? dirtUvMin : stoneUvMin);
                    const float* uvMax = blockType == BlockType::Grass ? grassTopUvMax : (blockType == BlockType::Dirt ? dirtUvMax : stoneUvMax);
                    AppendFace(chunk, uvMin, uvMax, color, vertices);
                }

                if (world.IsFaceVisible(worldX, worldY, worldZ, 0, 0, -1))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ),
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ)
                    };
                    const float* uvMin = blockType == BlockType::Grass ? grassSideUvMin : (blockType == BlockType::Dirt ? dirtUvMin : stoneUvMin);
                    const float* uvMax = blockType == BlockType::Grass ? grassSideUvMax : (blockType == BlockType::Dirt ? dirtUvMax : stoneUvMax);
                    AppendFace(chunk, uvMin, uvMax, sideColor, vertices);
                }

                if (world.IsFaceVisible(worldX, worldY, worldZ, 1, 0, 0))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ)
                    };
                    const float color[3] = {sideColor[0] * 0.95f, sideColor[1] * 0.95f, sideColor[2] * 0.95f};
                    const float* uvMin = blockType == BlockType::Grass ? grassSideUvMin : (blockType == BlockType::Dirt ? dirtUvMin : stoneUvMin);
                    const float* uvMax = blockType == BlockType::Grass ? grassSideUvMax : (blockType == BlockType::Dirt ? dirtUvMax : stoneUvMax);
                    AppendFace(chunk, uvMin, uvMax, color, vertices);
                }

                if (world.IsFaceVisible(worldX, worldY, worldZ, 0, 0, 1))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX + 1), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX + 1), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1)
                    };
                    const float color[3] = {sideColor[0] * 0.90f, sideColor[1] * 0.90f, sideColor[2] * 0.90f};
                    const float* uvMin = blockType == BlockType::Grass ? grassSideUvMin : (blockType == BlockType::Dirt ? dirtUvMin : stoneUvMin);
                    const float* uvMax = blockType == BlockType::Grass ? grassSideUvMax : (blockType == BlockType::Dirt ? dirtUvMax : stoneUvMax);
                    AppendFace(chunk, uvMin, uvMax, color, vertices);
                }

                if (world.IsFaceVisible(worldX, worldY, worldZ, -1, 0, 0))
                {
                    const float vertices[12] = {
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ + 1),
                        static_cast<float>(worldX), static_cast<float>(worldY), static_cast<float>(worldZ),
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ),
                        static_cast<float>(worldX), static_cast<float>(worldY + 1), static_cast<float>(worldZ + 1)
                    };
                    const float color[3] = {sideColor[0] * 0.85f, sideColor[1] * 0.85f, sideColor[2] * 0.85f};
                    const float* uvMin = blockType == BlockType::Grass ? grassSideUvMin : (blockType == BlockType::Dirt ? dirtUvMin : stoneUvMin);
                    const float* uvMax = blockType == BlockType::Grass ? grassSideUvMax : (blockType == BlockType::Dirt ? dirtUvMax : stoneUvMax);
                    AppendFace(chunk, uvMin, uvMax, color, vertices);
                }
            }
        }
    }
}

void ChunkMesher::AppendFace(
    Chunk& chunk,
    const float uvMin[2],
    const float uvMax[2],
    const float color[3],
    const float vertices[12]
)
{
    const float uvs[8] = {
        uvMin[0], uvMax[1],
        uvMax[0], uvMax[1],
        uvMax[0], uvMin[1],
        uvMin[0], uvMin[1]
    };

    for (int vertexIndex = 0; vertexIndex < 4; ++vertexIndex)
    {
        MeshVertex vertex;
        vertex.x = vertices[vertexIndex * 3];
        vertex.y = vertices[vertexIndex * 3 + 1];
        vertex.z = vertices[vertexIndex * 3 + 2];
        vertex.u = uvs[vertexIndex * 2];
        vertex.v = uvs[vertexIndex * 2 + 1];
        vertex.r = color[0];
        vertex.g = color[1];
        vertex.b = color[2];
        chunk.AddMeshVertex(vertex);
    }
}
