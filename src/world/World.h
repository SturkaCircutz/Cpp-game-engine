#pragma once

#include "math/VecMath.h"
#include "render/TextureAtlas.h"
#include "world/ChunkMesher.h"
#include "world/Chunk.h"
#include "world/TerrainGenerator.h"

#include <memory>
#include <unordered_map>

struct ChunkCoord
{
    int x = 0;
    int z = 0;

    bool operator==(const ChunkCoord& other) const;
};

struct ChunkCoordHash
{
    size_t operator()(const ChunkCoord& coord) const;
};

class World
{
public:
    World();
    ~World();

    bool InitializeRendering();
    void ShutdownRendering();

    void Render(const Vec3& cameraPosition) const;
    bool IntersectsSolid(const Vec3& minCorner, const Vec3& maxCorner) const;
    int GetSurfaceHeight(int worldX, int worldZ) const;

private:
    void GenerateInitialWorld();
    void GenerateChunk(Chunk& chunk);
    void RebuildChunkMeshes();
    bool UploadChunkMesh(Chunk& chunk);
    const Chunk* FindChunk(int chunkX, int chunkZ) const;
    Chunk* FindChunk(int chunkX, int chunkZ);
    BlockType GetBlock(int worldX, int worldY, int worldZ) const;
    bool IsSolidBlock(int x, int y, int z) const;
public:
    bool IsFaceVisible(int x, int y, int z, int offsetX, int offsetY, int offsetZ) const;
private:

    std::unordered_map<ChunkCoord, std::unique_ptr<Chunk>, ChunkCoordHash> chunks;
    TerrainGenerator terrainGenerator;
    ChunkMesher chunkMesher;
    TextureAtlas atlas;
    bool renderingInitialized = false;
};
