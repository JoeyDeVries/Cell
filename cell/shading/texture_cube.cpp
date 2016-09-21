#include "texture_cube.h"

namespace Cell
{
    TextureCube::TextureCube()
    {

    }

    TextureCube::~TextureCube()
    {

    }

    // NOTE(Joey): cubemap texture generation
    void TextureCube::Generate(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, unsigned char **data)
    {
        size_t stride = 1;
        if(type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_HALF_FLOAT)
            stride = 2;
        if(type == GL_FLOAT) 
            stride = 4;

        Bind();
        for (unsigned int face = 0; face < 6; ++face)
        {
            size_t dataOffset = face * width * height * stride;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, 0, Format, width, height, 0, dataFormat, type, data + dataOffset);
        }
    }

    void TextureCube::SetMipLayer(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, unsigned int mipLevel, unsigned char **data)
    {
        size_t stride = 1;
        if (type == GL_SHORT || type == GL_UNSIGNED_SHORT || type == GL_HALF_FLOAT)
            stride = 2;
        if (type == GL_FLOAT)
            stride = 4;

        Bind();
        for (unsigned int face = 0; face < 6; ++face)
        {
            size_t dataOffset = face * width * height * stride;
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipLevel, 0, 0, width, height, dataFormat, type, data + dataOffset);
        }
    }

    void TextureCube::Bind(int unit)
    {
        if (unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);
    }

    void TextureCube::Unbind()
    {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}