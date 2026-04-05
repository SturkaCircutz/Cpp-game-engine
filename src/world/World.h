#pragma once

#include "core/Math.h"

#include <unordered_map>
#include <vector>

struct BlockCoord
{
    int x = 0;
    int y = 0;
    int z = 0;

    bool operator==(const BlockCoord& other) const;
};

struct RaycastHit
{
    BlockCoord block;
    BlockCoord adjacent;
    Vec3 normal;
};

struct ChunkCoord
{
    int x = 0;
    int z = 0;
};

class World
{
public:
    World();

    void EnsureChunksAround(const Vec3& position);
    bool AddBlock(const BlockCoord& block);
    bool RemoveBlock(const BlockCoord& block);
    bool HasBlock(const BlockCoord& block) const;
    bool Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const;
    const std::vector<BlockCoord>& GetBlocks() const;
    float GetMinCameraHeight() const;
    float GetMaxCameraHeight() const;

private:
    void LoadChunk(const ChunkCoord& chunk);
    void UnloadFarChunks(const ChunkCoord& centerChunk);
    void RebuildVisibleBlocks();
    bool IsGeneratedBlock(const BlockCoord& block) const;
    bool IsChunkLoaded(const ChunkCoord& chunk) const;

    std::unordered_map<long long, std::vector<BlockCoord>> chunks;
    std::unordered_map<long long, bool> blockOverrides;
    std::vector<BlockCoord> visibleBlocks;
};
