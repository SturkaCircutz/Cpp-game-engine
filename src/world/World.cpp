#include "world/World.h"
#include "world/WorldRenderApi.h"

#include <GLFW/glfw3.h>

#include <cmath>

namespace
{
    GlGenBuffersProc gGlGenBuffers = nullptr;
    GlBindBufferProc gGlBindBuffer = nullptr;
    GlBufferDataProc gGlBufferData = nullptr;
    GlDeleteBuffersProc gGlDeleteBuffers = nullptr;

    constexpr int kWorldChunkRadius = 2;
    constexpr int kRenderChunkRadius = 1;
}

bool ChunkCoord::operator==(const ChunkCoord& other) const
{
    return x == other.x && z == other.z;
}

size_t ChunkCoordHash::operator()(const ChunkCoord& coord) const
{
    const size_t xHash = std::hash<int>{}(coord.x);
    const size_t zHash = std::hash<int>{}(coord.z);
    return xHash ^ (zHash << 1);
}

World::World()
{
    GenerateInitialWorld();
}

World::~World()
{
    ShutdownRendering();
}

bool World::InitializeRendering()
{
    gGlGenBuffers = reinterpret_cast<GlGenBuffersProc>(glfwGetProcAddress("glGenBuffers"));
    gGlBindBuffer = reinterpret_cast<GlBindBufferProc>(glfwGetProcAddress("glBindBuffer"));
    gGlBufferData = reinterpret_cast<GlBufferDataProc>(glfwGetProcAddress("glBufferData"));
    gGlDeleteBuffers = reinterpret_cast<GlDeleteBuffersProc>(glfwGetProcAddress("glDeleteBuffers"));

    if (gGlGenBuffers == nullptr || gGlBindBuffer == nullptr || gGlBufferData == nullptr || gGlDeleteBuffers == nullptr)
    {
        return false;
    }

    if (!atlas.LoadFromPpm("assets/textures/block_atlas.ppm"))
    {
        return false;
    }

    for (auto& entry : chunks)
    {
        if (!UploadChunkMesh(*entry.second))
        {
            return false;
        }
    }

    renderingInitialized = true;
    return true;
}

void World::ShutdownRendering()
{
    if (!renderingInitialized || gGlDeleteBuffers == nullptr)
    {
        return;
    }

    for (auto& entry : chunks)
    {
        Chunk& chunk = *entry.second;
        const GLuint bufferId = static_cast<GLuint>(chunk.GetVertexBufferId());
        if (bufferId != 0)
        {
            gGlDeleteBuffers(1, &bufferId);
            chunk.SetVertexBufferId(0);
            chunk.SetVertexCount(0);
        }
    }

    renderingInitialized = false;
    atlas.Destroy();
}

void World::Render(const Vec3& cameraPosition) const
{
    if (!renderingInitialized)
    {
        return;
    }

    const int cameraChunkX = static_cast<int>(std::floor(cameraPosition.x / static_cast<float>(Chunk::kSizeX)));
    const int cameraChunkZ = static_cast<int>(std::floor(cameraPosition.z / static_cast<float>(Chunk::kSizeZ)));

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    atlas.Bind();

    for (const auto& entry : chunks)
    {
        const Chunk& chunk = *entry.second;

        const int chunkDistanceX = std::abs(chunk.GetChunkX() - cameraChunkX);
        const int chunkDistanceZ = std::abs(chunk.GetChunkZ() - cameraChunkZ);
        if (chunkDistanceX > kRenderChunkRadius || chunkDistanceZ > kRenderChunkRadius)
        {
            continue;
        }

        const GLuint bufferId = static_cast<GLuint>(chunk.GetVertexBufferId());
        if (bufferId == 0 || chunk.GetVertexCount() == 0)
        {
            continue;
        }

        gGlBindBuffer(GL_ARRAY_BUFFER, bufferId);
        glVertexPointer(3, GL_FLOAT, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, x)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, u)));
        glColorPointer(3, GL_FLOAT, sizeof(MeshVertex), reinterpret_cast<const void*>(offsetof(MeshVertex, r)));
        glDrawArrays(GL_QUADS, 0, chunk.GetVertexCount());
    }

    gGlBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void World::GenerateInitialWorld()
{
    for (int chunkZ = -kWorldChunkRadius; chunkZ <= kWorldChunkRadius; ++chunkZ)
    {
        for (int chunkX = -kWorldChunkRadius; chunkX <= kWorldChunkRadius; ++chunkX)
        {
            auto chunk = std::make_unique<Chunk>(chunkX, chunkZ);
            GenerateChunk(*chunk);
            chunks[{chunkX, chunkZ}] = std::move(chunk);
        }
    }

    RebuildChunkMeshes();
}

void World::GenerateChunk(Chunk& chunk)
{
    terrainGenerator.FillChunk(chunk);
}

bool World::IntersectsSolid(const Vec3& minCorner, const Vec3& maxCorner) const
{
    const int minX = static_cast<int>(std::floor(minCorner.x));
    const int maxX = static_cast<int>(std::ceil(maxCorner.x)) - 1;
    const int minY = static_cast<int>(std::floor(minCorner.y));
    const int maxY = static_cast<int>(std::ceil(maxCorner.y)) - 1;
    const int minZ = static_cast<int>(std::floor(minCorner.z));
    const int maxZ = static_cast<int>(std::ceil(maxCorner.z)) - 1;

    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            for (int z = minZ; z <= maxZ; ++z)
            {
                if (IsSolidBlock(x, y, z))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void World::RebuildChunkMeshes()
{
    for (auto& entry : chunks)
    {
        chunkMesher.BuildChunkMesh(*entry.second, *this);
    }
}

bool World::UploadChunkMesh(Chunk& chunk)
{
    const GLuint existingBufferId = static_cast<GLuint>(chunk.GetVertexBufferId());
    if (existingBufferId != 0)
    {
        gGlDeleteBuffers(1, &existingBufferId);
        chunk.SetVertexBufferId(0);
    }

    const std::vector<MeshVertex>& meshVertices = chunk.GetMeshVertices();
    if (meshVertices.empty())
    {
        chunk.SetVertexCount(0);
        return true;
    }

    GLuint bufferId = 0;
    gGlGenBuffers(1, &bufferId);
    if (bufferId == 0)
    {
        return false;
    }

    gGlBindBuffer(GL_ARRAY_BUFFER, bufferId);
    gGlBufferData(
        GL_ARRAY_BUFFER,
        static_cast<ptrdiff_t>(meshVertices.size() * sizeof(MeshVertex)),
        meshVertices.data(),
        GL_STATIC_DRAW
    );
    gGlBindBuffer(GL_ARRAY_BUFFER, 0);

    chunk.SetVertexBufferId(bufferId);
    chunk.SetVertexCount(static_cast<int>(meshVertices.size()));
    return true;
}

int World::GetSurfaceHeight(int worldX, int worldZ) const
{
    for (int worldY = Chunk::kSizeY - 1; worldY >= 0; --worldY)
    {
        if (IsSolidBlock(worldX, worldY, worldZ))
        {
            return worldY;
        }
    }

    return 0;
}

const Chunk* World::FindChunk(int chunkX, int chunkZ) const
{
    const auto iterator = chunks.find({chunkX, chunkZ});
    if (iterator == chunks.end())
    {
        return nullptr;
    }

    return iterator->second.get();
}

Chunk* World::FindChunk(int chunkX, int chunkZ)
{
    const auto iterator = chunks.find({chunkX, chunkZ});
    if (iterator == chunks.end())
    {
        return nullptr;
    }

    return iterator->second.get();
}

BlockType World::GetBlock(int worldX, int worldY, int worldZ) const
{
    if (worldY < 0 || worldY >= Chunk::kSizeY)
    {
        return BlockType::Air;
    }

    const int chunkX = static_cast<int>(std::floor(static_cast<float>(worldX) / static_cast<float>(Chunk::kSizeX)));
    const int chunkZ = static_cast<int>(std::floor(static_cast<float>(worldZ) / static_cast<float>(Chunk::kSizeZ)));
    const Chunk* chunk = FindChunk(chunkX, chunkZ);
    if (chunk == nullptr)
    {
        return BlockType::Air;
    }

    const int localX = ((worldX % Chunk::kSizeX) + Chunk::kSizeX) % Chunk::kSizeX;
    const int localZ = ((worldZ % Chunk::kSizeZ) + Chunk::kSizeZ) % Chunk::kSizeZ;
    return chunk->GetBlock(localX, worldY, localZ);
}

bool World::IsSolidBlock(int x, int y, int z) const
{
    return GetBlock(x, y, z) != BlockType::Air;
}

bool World::IsFaceVisible(int x, int y, int z, int offsetX, int offsetY, int offsetZ) const
{
    return !IsSolidBlock(x + offsetX, y + offsetY, z + offsetZ);
}
