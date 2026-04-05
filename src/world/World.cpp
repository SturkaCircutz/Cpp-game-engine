#include "world/World.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#endif

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_set>

bool BlockCoord::operator==(const BlockCoord& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

namespace
{
    constexpr int kChunkSize = 16;
    constexpr int kRenderChunkRadius = 3;
    constexpr int kTerrainFloor = -4;
    constexpr int kBaseTerrainHeight = 2;
    constexpr int kTerrainHeightRange = 10;
    constexpr float kRayEpsilon = 0.0001f;
    constexpr float kCameraMinY = 0.75f;
    constexpr float kCameraMaxY = 96.0f;

    long long MakeChunkKey(int chunkX, int chunkZ)
    {
        return (static_cast<long long>(chunkX) << 32) ^ static_cast<unsigned int>(chunkZ);
    }

    long long MakeBlockKey(int x, int y, int z)
    {
        const long long packedX = static_cast<unsigned int>(x) & 0x1FFFFF;
        const long long packedY = static_cast<unsigned int>(y) & 0x1FFFFF;
        const long long packedZ = static_cast<unsigned int>(z) & 0x1FFFFF;
        return (packedX << 42) | (packedY << 21) | packedZ;
    }

    int FloorDiv(int value, int divisor)
    {
        int quotient = value / divisor;
        const int remainder = value % divisor;
        if (remainder != 0 && ((remainder < 0) != (divisor < 0)))
        {
            --quotient;
        }

        return quotient;
    }

    ChunkCoord GetChunkCoordForBlock(const BlockCoord& block)
    {
        return {FloorDiv(block.x, kChunkSize), FloorDiv(block.z, kChunkSize)};
    }

    ChunkCoord GetChunkCoordForPosition(const Vec3& position)
    {
        return {
            FloorDiv(static_cast<int>(std::floor(position.x)), kChunkSize),
            FloorDiv(static_cast<int>(std::floor(position.z)), kChunkSize)
        };
    }

    float Fade(float value)
    {
        return value * value * value * (value * (value * 6.0f - 15.0f) + 10.0f);
    }

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    float RandomUnit(int x, int z, int seed)
    {
        unsigned int value = static_cast<unsigned int>(x) * 374761393u +
            static_cast<unsigned int>(z) * 668265263u +
            static_cast<unsigned int>(seed) * 1442695041u;
        value = (value ^ (value >> 13)) * 1274126177u;
        value ^= value >> 16;
        return static_cast<float>(value & 0xFFFFu) / 65535.0f;
    }

    float ValueNoise2D(float x, float z, int seed)
    {
        const int x0 = static_cast<int>(std::floor(x));
        const int z0 = static_cast<int>(std::floor(z));
        const int x1 = x0 + 1;
        const int z1 = z0 + 1;

        const float tx = Fade(x - static_cast<float>(x0));
        const float tz = Fade(z - static_cast<float>(z0));

        const float v00 = RandomUnit(x0, z0, seed);
        const float v10 = RandomUnit(x1, z0, seed);
        const float v01 = RandomUnit(x0, z1, seed);
        const float v11 = RandomUnit(x1, z1, seed);

        const float ix0 = Lerp(v00, v10, tx);
        const float ix1 = Lerp(v01, v11, tx);
        return Lerp(ix0, ix1, tz);
    }

    float FractalNoise2D(float x, float z)
    {
        float amplitude = 1.0f;
        float frequency = 0.045f;
        float total = 0.0f;
        float normalization = 0.0f;

        for (int octave = 0; octave < 4; ++octave)
        {
            total += ValueNoise2D(x * frequency, z * frequency, 1337 + octave * 97) * amplitude;
            normalization += amplitude;
            amplitude *= 0.5f;
            frequency *= 2.0f;
        }

        return normalization > 0.0f ? total / normalization : 0.0f;
    }

    int GetTerrainHeight(int x, int z)
    {
        const float noise = FractalNoise2D(static_cast<float>(x), static_cast<float>(z));
        return kBaseTerrainHeight + static_cast<int>(std::round(noise * static_cast<float>(kTerrainHeightRange)));
    }

    bool IntersectAxis(float origin, float direction, float minValue, float maxValue, float& tMin, float& tMax)
    {
        if (std::abs(direction) <= kRayEpsilon)
        {
            return origin >= minValue && origin <= maxValue;
        }

        const float inverseDirection = 1.0f / direction;
        float axisMin = (minValue - origin) * inverseDirection;
        float axisMax = (maxValue - origin) * inverseDirection;
        if (axisMin > axisMax)
        {
            std::swap(axisMin, axisMax);
        }

        tMin = std::max(tMin, axisMin);
        tMax = std::min(tMax, axisMax);
        return tMin <= tMax;
    }

    Vec3 ComputeHitNormal(const Vec3& hitPoint, const BlockCoord& block)
    {
        const float minX = static_cast<float>(block.x);
        const float maxX = minX + 1.0f;
        const float minY = static_cast<float>(block.y);
        const float maxY = minY + 1.0f;
        const float minZ = static_cast<float>(block.z);
        const float maxZ = minZ + 1.0f;

        if (std::abs(hitPoint.x - minX) <= 0.01f)
        {
            return Vec3(-1.0f, 0.0f, 0.0f);
        }
        if (std::abs(hitPoint.x - maxX) <= 0.01f)
        {
            return Vec3(1.0f, 0.0f, 0.0f);
        }
        if (std::abs(hitPoint.y - minY) <= 0.01f)
        {
            return Vec3(0.0f, -1.0f, 0.0f);
        }
        if (std::abs(hitPoint.y - maxY) <= 0.01f)
        {
            return Vec3(0.0f, 1.0f, 0.0f);
        }
        if (std::abs(hitPoint.z - minZ) <= 0.01f)
        {
            return Vec3(0.0f, 0.0f, -1.0f);
        }

        return Vec3(0.0f, 0.0f, 1.0f);
    }
}

World::World()
{
}

void World::EnsureChunksAround(const Vec3& position)
{
    const ChunkCoord centerChunk = GetChunkCoordForPosition(position);

    for (int chunkZ = centerChunk.z - kRenderChunkRadius; chunkZ <= centerChunk.z + kRenderChunkRadius; ++chunkZ)
    {
        for (int chunkX = centerChunk.x - kRenderChunkRadius; chunkX <= centerChunk.x + kRenderChunkRadius; ++chunkX)
        {
            LoadChunk({chunkX, chunkZ});
        }
    }

    UnloadFarChunks(centerChunk);
    RebuildVisibleBlocks();
}

bool World::AddBlock(const BlockCoord& block)
{
    if (HasBlock(block))
    {
        return false;
    }

    blockOverrides[MakeBlockKey(block.x, block.y, block.z)] = true;
    RebuildVisibleBlocks();
    return true;
}

bool World::RemoveBlock(const BlockCoord& block)
{
    if (!HasBlock(block))
    {
        return false;
    }

    blockOverrides[MakeBlockKey(block.x, block.y, block.z)] = false;
    RebuildVisibleBlocks();
    return true;
}

bool World::HasBlock(const BlockCoord& block) const
{
    const auto overrideIt = blockOverrides.find(MakeBlockKey(block.x, block.y, block.z));
    if (overrideIt != blockOverrides.end())
    {
        return overrideIt->second;
    }

    return IsGeneratedBlock(block);
}

bool World::Raycast(const Vec3& origin, const Vec3& direction, float maxDistance, RaycastHit& hit) const
{
    const Vec3 ray = Normalize(direction);
    if (Length(ray) <= kRayEpsilon)
    {
        return false;
    }

    BlockCoord currentCell = {
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    };
    BlockCoord previousCell = currentCell;

    const int stepX = ray.x > 0.0f ? 1 : (ray.x < 0.0f ? -1 : 0);
    const int stepY = ray.y > 0.0f ? 1 : (ray.y < 0.0f ? -1 : 0);
    const int stepZ = ray.z > 0.0f ? 1 : (ray.z < 0.0f ? -1 : 0);

    auto intBound = [](float start, float delta) -> float
    {
        if (std::abs(delta) <= kRayEpsilon)
        {
            return std::numeric_limits<float>::infinity();
        }

        if (delta > 0.0f)
        {
            return (std::floor(start) + 1.0f - start) / delta;
        }

        return (start - std::floor(start)) / -delta;
    };

    float tMaxX = intBound(origin.x, ray.x);
    float tMaxY = intBound(origin.y, ray.y);
    float tMaxZ = intBound(origin.z, ray.z);

    const float tDeltaX = stepX == 0 ? std::numeric_limits<float>::infinity() : 1.0f / std::abs(ray.x);
    const float tDeltaY = stepY == 0 ? std::numeric_limits<float>::infinity() : 1.0f / std::abs(ray.y);
    const float tDeltaZ = stepZ == 0 ? std::numeric_limits<float>::infinity() : 1.0f / std::abs(ray.z);

    float traveled = 0.0f;
    while (traveled <= maxDistance)
    {
        previousCell = currentCell;

        if (tMaxX < tMaxY && tMaxX < tMaxZ)
        {
            currentCell.x += stepX;
            traveled = tMaxX;
            tMaxX += tDeltaX;
        }
        else if (tMaxY < tMaxZ)
        {
            currentCell.y += stepY;
            traveled = tMaxY;
            tMaxY += tDeltaY;
        }
        else
        {
            currentCell.z += stepZ;
            traveled = tMaxZ;
            tMaxZ += tDeltaZ;
        }

        if (traveled > maxDistance)
        {
            break;
        }

        if (HasBlock(currentCell))
        {
            const Vec3 hitPoint = origin + ray * traveled;
            hit.block = currentCell;
            hit.normal = ComputeHitNormal(hitPoint, currentCell);
            hit.adjacent = {
                currentCell.x + static_cast<int>(hit.normal.x),
                currentCell.y + static_cast<int>(hit.normal.y),
                currentCell.z + static_cast<int>(hit.normal.z)
            };
            return true;
        }
    }

    return false;
}

const std::vector<BlockCoord>& World::GetBlocks() const
{
    return visibleBlocks;
}

float World::GetMinCameraHeight() const
{
    return kCameraMinY;
}

float World::GetMaxCameraHeight() const
{
    return kCameraMaxY;
}

void World::LoadChunk(const ChunkCoord& chunk)
{
    if (IsChunkLoaded(chunk))
    {
        return;
    }

    std::vector<BlockCoord> blocksInChunk;
    blocksInChunk.reserve(kChunkSize * kChunkSize * 8);

    const int startX = chunk.x * kChunkSize;
    const int startZ = chunk.z * kChunkSize;

    for (int localZ = 0; localZ < kChunkSize; ++localZ)
    {
        for (int localX = 0; localX < kChunkSize; ++localX)
        {
            const int worldX = startX + localX;
            const int worldZ = startZ + localZ;
            const int height = GetTerrainHeight(worldX, worldZ);

            for (int y = kTerrainFloor; y <= height; ++y)
            {
                blocksInChunk.push_back({worldX, y, worldZ});
            }
        }
    }

    chunks.emplace(MakeChunkKey(chunk.x, chunk.z), std::move(blocksInChunk));
}

void World::UnloadFarChunks(const ChunkCoord& centerChunk)
{
    std::unordered_set<long long> keepKeys;
    for (int chunkZ = centerChunk.z - kRenderChunkRadius; chunkZ <= centerChunk.z + kRenderChunkRadius; ++chunkZ)
    {
        for (int chunkX = centerChunk.x - kRenderChunkRadius; chunkX <= centerChunk.x + kRenderChunkRadius; ++chunkX)
        {
            keepKeys.insert(MakeChunkKey(chunkX, chunkZ));
        }
    }

    for (auto it = chunks.begin(); it != chunks.end();)
    {
        if (keepKeys.find(it->first) == keepKeys.end())
        {
            it = chunks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void World::RebuildVisibleBlocks()
{
    visibleBlocks.clear();

    for (const auto& chunkEntry : chunks)
    {
        for (const BlockCoord& block : chunkEntry.second)
        {
            const auto overrideIt = blockOverrides.find(MakeBlockKey(block.x, block.y, block.z));
            if (overrideIt != blockOverrides.end() && !overrideIt->second)
            {
                continue;
            }

            visibleBlocks.push_back(block);
        }
    }

    for (const auto& overrideEntry : blockOverrides)
    {
        if (!overrideEntry.second)
        {
            continue;
        }

        const long long key = overrideEntry.first;
        const int x = static_cast<int>((key >> 42) & 0x1FFFFF);
        const int y = static_cast<int>((key >> 21) & 0x1FFFFF);
        const int z = static_cast<int>(key & 0x1FFFFF);

        auto unpack = [](int value) -> int
        {
            return value & (1 << 20) ? value | ~0x1FFFFF : value;
        };

        const BlockCoord block = {unpack(x), unpack(y), unpack(z)};
        if (!IsGeneratedBlock(block))
        {
            visibleBlocks.push_back(block);
        }
    }
}

bool World::IsGeneratedBlock(const BlockCoord& block) const
{
    const ChunkCoord chunk = GetChunkCoordForBlock(block);
    const auto it = chunks.find(MakeChunkKey(chunk.x, chunk.z));
    if (it == chunks.end())
    {
        return false;
    }

    for (const BlockCoord& candidate : it->second)
    {
        if (candidate == block)
        {
            return true;
        }
    }

    return false;
}

bool World::IsChunkLoaded(const ChunkCoord& chunk) const
{
    return chunks.find(MakeChunkKey(chunk.x, chunk.z)) != chunks.end();
}
