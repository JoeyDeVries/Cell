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
    void TextureCube::GenerateFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned char *data)
    {
        // TODO(Joey): better way to generate cubemap (not per-face perhaps, but a total call?)
        if(FaceWidth == 0)
            glGenTextures(1, &m_ID);

        FaceWidth = width;
        FaceHeight = height;
        Format = format;
        Type = type;

        Bind();

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, FilterMin);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, FilterMax);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, WrapS);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, WrapT);

        glTexImage2D(face, 0, format, width, height, 0, format, type, data);
    }

    void TextureCube::SetMipFace(GLenum face, unsigned int width, unsigned int height, GLenum format, GLenum type, unsigned int mipLevel, unsigned char *data)
    {
        Bind();
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, mipLevel, 0, 0, width, height, format, type, data);
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