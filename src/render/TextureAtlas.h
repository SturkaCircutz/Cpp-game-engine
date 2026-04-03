#pragma once

#include <cstdint>
#include <string>

class TextureAtlas
{
public:
    ~TextureAtlas();

    bool LoadFromPpm(const std::string& path);
    void Bind() const;
    void Destroy();
    std::uint32_t GetTextureId() const;

private:
    std::uint32_t textureId = 0;
};
