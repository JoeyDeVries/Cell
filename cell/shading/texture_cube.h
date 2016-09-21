#ifndef CELL_SHADING_TEXTURE_CUBE_H
#define CELL_SHADING_TEXTURE_CUBE_H

#include "texture.h"

#include <GL/glew.h>

#include <vector>

namespace Cell
{
    class TextureCube
    {
    public:
        // TODO(Joey): these should be private and only accessed w/ getters/setters s.t. we can 
        // directly change the texture state where relevant from within the setters.
        GLenum Format = GL_RGBA;                 // NOTE(Joey): the format each texel is stored in
        GLenum FilterMin = GL_LINEAR_MIPMAP_LINEAR; // NOTE(Joey): what filter method to use during minification
        GLenum FilterMax = GL_LINEAR;               // NOTE(Joey): what filter method to use during magnification
        GLenum WrapS = GL_REPEAT;               // NOTE(Joey): wrapping method of the S coordinate
        GLenum WrapT = GL_REPEAT;               // NOTE(Joey): wrapping method of the T coordinate
        GLenum WrapR = GL_REPEAT;               // NOTE(Joey): wrapping method of the R coordinate
        bool   Mipmapping = true;
    private:
        unsigned int m_ID;
    public:
        TextureCube();
        ~TextureCube();

        // NOTE(Joey): cubemap texture generation
        void Generate(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, unsigned char **data);
        //void Generate(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, std::vector<Texture> faces);
        void SetMipLayer(unsigned int width, unsigned int height, GLenum dataFormat, GLenum type, unsigned int mipLevel, unsigned char **data);

        void Bind(int unit = -1);
        void Unbind();
    };
}
#endif