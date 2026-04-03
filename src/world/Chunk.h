#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

enum class BlockType
{
    Air = 0,
    Grass,
    Dirt,
    Stone
};

struct MeshVertex
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    float u = 0.0f;
    float v = 0.0f;
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
};

class Chunk
{
public:
    static constexpr int kSizeX = 16;
    static constexpr int kSizeY = 32;
    static constexpr int kSizeZ = 16;

    Chunk(int chunkXValue, int chunkZValue);

    int GetChunkX() const;
    int GetChunkZ() const;

    BlockType GetBlock(int localX, int localY, int localZ) const;
    void SetBlock(int localX, int localY, int localZ, BlockType blockType);
    void ClearMesh();
    void AddMeshVertex(const MeshVertex& vertex);
    const std::vector<MeshVertex>& GetMeshVertices() const;
    void SetVertexCount(int count);
    int GetVertexCount() const;
    void SetVertexBufferId(std::uint32_t bufferId);
    std::uint32_t GetVertexBufferId() const;

private:
    int GetIndex(int localX, int localY, int localZ) const;

    int chunkX = 0;
    int chunkZ = 0;
    std::vector<BlockType> blocks;
    std::vector<MeshVertex> meshVertices;
    int vertexCount = 0;
    std::uint32_t vertexBufferId = 0;
};
