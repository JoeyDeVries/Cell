#include "texture.h"

#include <assert.h>

#include <utility/logging/log.h>

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Texture::Texture()
    {
        
    }
    // --------------------------------------------------------------------------------------------
    Texture::~Texture()
    {

    }
    // --------------------------------------------------------------------------------------------
    void Texture::Generate(unsigned int width, GLenum internalFormat, GLenum format, GLenum type, void* data)
    {
        glGenTextures(1, &ID);

        Width          = width;
        Height         = 0;
        Depth          = 0;
        InternalFormat = internalFormat;
        Format         = format;
        Type           = type;

        assert(Target == GL_TEXTURE_1D);
        Bind();
            glTexImage1D(Target, 0, internalFormat, width, 0, format, type, data);
            glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, WrapS);
            if(Mipmapping)
                glGenerateMipmap(Target);
        Unbind();
    }
    // --------------------------------------------------------------------------------------------
    void Texture::Generate(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data)
    {
        glGenTextures(1, &ID);

        Width          = width;
        Height         = height;
        Depth          = 0;
        InternalFormat = internalFormat;
        Format         = format;
        Type           = type;

        assert(Target == GL_TEXTURE_2D);
        Bind();
            glTexImage2D(Target, 0, internalFormat, width, height, 0, format, type, data);
            glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, WrapS);
            glTexParameteri(Target, GL_TEXTURE_WRAP_T, WrapT);
            if (Mipmapping)
                glGenerateMipmap(Target);
        Unbind();
    }
    // --------------------------------------------------------------------------------------------
    void Texture::Generate(unsigned int width, unsigned int height, unsigned int depth, GLenum internalFormat, GLenum format, GLenum type, void* data)
    {
        glGenTextures(1, &ID);

        Width          = width;
        Height         = height;
        Depth          = depth;
        InternalFormat = internalFormat;
        Format         = format;
        Type           = type;

        assert(Target == GL_TEXTURE_3D);
        Bind();
            glTexImage3D(Target, 0, internalFormat, width, height, depth, 0, format, type, data);
            glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, FilterMin);
            glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, FilterMax);
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, WrapS);
            glTexParameteri(Target, GL_TEXTURE_WRAP_T, WrapT);
            glTexParameteri(Target, GL_TEXTURE_WRAP_R, WrapR);
            if (Mipmapping)
                glGenerateMipmap(Target);
        Unbind();
    }
    // --------------------------------------------------------------------------------------------
    void Texture::Resize(unsigned int width, unsigned int height, unsigned int depth)
    {
        Bind();
        if (Target == GL_TEXTURE_1D)
        {
            glTexImage1D(GL_TEXTURE_1D, 0, InternalFormat, width, 0, Format, Type, 0);
        }
        else if (Target == GL_TEXTURE_2D)
        {
            assert(height > 0);
            glTexImage2D(GL_TEXTURE_2D, 0, InternalFormat, width, height, 0, Format, Type, 0);
        }
        else if (Target == GL_TEXTURE_3D)
        {
            assert(height > 0 && depth > 0);
            glTexImage3D(GL_TEXTURE_3D, 0, InternalFormat, width, height, depth, 0, Format, Type, 0);
        }
    }
    // --------------------------------------------------------------------------------------------
    void Texture::Bind(int unit)
    {
        if(unit >= 0)
            glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(Target, ID);
    }
    // --------------------------------------------------------------------------------------------
    void Texture::Unbind()
    {
        glBindTexture(Target, 0);
    }
    // --------------------------------------------------------------------------------------------
    void Texture::SetWrapMode(GLenum wrapMode, bool bind)
    {
        if(bind)
            Bind();
        if (Target == GL_TEXTURE_1D)
        {
            WrapS = wrapMode;
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, wrapMode);
        }
        else if (Target == GL_TEXTURE_2D)
        {
            WrapS = wrapMode;
            WrapT = wrapMode;
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, wrapMode);
            glTexParameteri(Target, GL_TEXTURE_WRAP_T, wrapMode);
        }
        else if (Target == GL_TEXTURE_3D)
        {
            WrapS = wrapMode;
            WrapT = wrapMode;
            WrapR = wrapMode;
            glTexParameteri(Target, GL_TEXTURE_WRAP_S, wrapMode);
            glTexParameteri(Target, GL_TEXTURE_WRAP_T, wrapMode);
            glTexParameteri(Target, GL_TEXTURE_WRAP_R, wrapMode);
        }
    }
    // --------------------------------------------------------------------------------------------
    void Texture::SetFilterMin(GLenum filter, bool bind)
    {
        if(bind)
            Bind();
        glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, filter);
    }
    // --------------------------------------------------------------------------------------------
    void Texture::SetFilterMax(GLenum filter, bool bind)
    {
        if (bind)
            Bind();
        glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, filter);
    }
}
