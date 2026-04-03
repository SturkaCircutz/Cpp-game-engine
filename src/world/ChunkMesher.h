#pragma once

#include "world/Chunk.h"

class World;

class ChunkMesher
{
public:
    void BuildChunkMesh(Chunk& chunk, const World& world) const;

private:
    static void AppendFace(
        Chunk& chunk,
        const float uvMin[2],
        const float uvMax[2],
        const float color[3],
        const float vertices[12]
    );
};
