#include "world/Chunk.h"

Chunk::Chunk(int chunkXValue, int chunkZValue)
    : chunkX(chunkXValue), chunkZ(chunkZValue),
      blocks(static_cast<size_t>(kSizeX * kSizeY * kSizeZ), BlockType::Air)
{
}

int Chunk::GetChunkX() const
{
    return chunkX;
}

int Chunk::GetChunkZ() const
{
    return chunkZ;
}

BlockType Chunk::GetBlock(int localX, int localY, int localZ) const
{
    return blocks[static_cast<size_t>(GetIndex(localX, localY, localZ))];
}

void Chunk::SetBlock(int localX, int localY, int localZ, BlockType blockType)
{
    blocks[static_cast<size_t>(GetIndex(localX, localY, localZ))] = blockType;
}

void Chunk::ClearMesh()
{
    meshVertices.clear();
}

void Chunk::AddMeshVertex(const MeshVertex& vertex)
{
    meshVertices.push_back(vertex);
}

const std::vector<MeshVertex>& Chunk::GetMeshVertices() const
{
    return meshVertices;
}

void Chunk::SetVertexCount(int count)
{
    vertexCount = count;
}

int Chunk::GetVertexCount() const
{
    return vertexCount;
}

void Chunk::SetVertexBufferId(std::uint32_t bufferId)
{
    vertexBufferId = bufferId;
}

std::uint32_t Chunk::GetVertexBufferId() const
{
    return vertexBufferId;
}

int Chunk::GetIndex(int localX, int localY, int localZ) const
{
    return localX + (localZ * kSizeX) + (localY * kSizeX * kSizeZ);
}
