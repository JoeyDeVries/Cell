#include "texture.h"

#include <assert.h>

namespace Cell
{
    Texture::Texture()
    {
        glGenTextures(1, &m_ID);
    }
    Texture::~Texture()
    {
        glDeleteTextures(1, &m_ID);
    }

    void Texture::Generate(unsigned int width, GLenum dataFormat, unsigned char *data)
    {
        assert(Type == GL_TEXTURE_1D);
        Bind();
            glTexImage1D(Type, 0, Format, width, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Type, GL_TEXTURE_WRAP_S, WrapS);
            if(Mipmapping)
                glGenerateMipmap(Type);
        Unbind();
    }

    void Texture::Generate(unsigned int width, unsigned int height, GLenum dataFormat, unsigned char *data)
    {
        assert(Type == GL_TEXTURE_2D);
        Bind();
            glTexImage2D(Type, 0, Format, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Type, GL_TEXTURE_WRAP_S, WrapS);
            glTexParameteri(Type, GL_TEXTURE_WRAP_T, WrapT);
            if (Mipmapping)
                glGenerateMipmap(Type);
        Unbind();
    }

    void Texture::Generate(unsigned int width, unsigned int height, unsigned int depth, GLenum dataFormat, unsigned char *data)
    {
        assert(Type == GL_TEXTURE_3D);
        Bind();
            glTexImage3D(Type, 0, Format, width, height, depth, 0, dataFormat, GL_UNSIGNED_BYTE, data);
            glTexParameteri(Type, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Type, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Type, GL_TEXTURE_WRAP_S, WrapS);
            glTexParameteri(Type, GL_TEXTURE_WRAP_T, WrapT);
            glTexParameteri(Type, GL_TEXTURE_WRAP_R, WrapR);
            if (Mipmapping)
                glGenerateMipmap(Type);
        Unbind();
    }

    void Texture::Generate(unsigned int width, unsigned int height, GLenum dataFormat, unsigned char **data)
    {

    }

    void Texture::Bind(int unit)
    {
        if(unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(Type, m_ID);
    }

    void Texture::Unbind()
    {
        glBindTexture(Type, 0);
    }
}
