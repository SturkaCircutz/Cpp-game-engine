#include "render/TextureAtlas.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>

#include <fstream>
#include <vector>

TextureAtlas::~TextureAtlas()
{
    Destroy();
}

bool TextureAtlas::LoadFromPpm(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
    {
        return false;
    }

    std::string format;
    int width = 0;
    int height = 0;
    int maxValue = 0;
    input >> format >> width >> height >> maxValue;
    if (!input || format != "P3" || width <= 0 || height <= 0 || maxValue != 255)
    {
        return false;
    }

    std::vector<unsigned char> pixels(static_cast<size_t>(width * height * 3));
    for (size_t index = 0; index < pixels.size(); ++index)
    {
        int value = 0;
        input >> value;
        if (!input)
        {
            return false;
        }
        pixels[index] = static_cast<unsigned char>(value);
    }

    if (textureId == 0)
    {
        glGenTextures(1, reinterpret_cast<GLuint*>(&textureId));
    }

    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(textureId));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        width,
        height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        pixels.data()
    );

    return true;
}

void TextureAtlas::Bind() const
{
    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(textureId));
}

void TextureAtlas::Destroy()
{
    if (textureId != 0)
    {
        glDeleteTextures(1, reinterpret_cast<const GLuint*>(&textureId));
        textureId = 0;
    }
}

std::uint32_t TextureAtlas::GetTextureId() const
{
    return textureId;
}
