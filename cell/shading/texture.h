#ifndef CELL_SHADING_TEXTURE_H
#define CELL_SHADING_TEXTURE_H

#include <GL/glew.h>

namespace Cell
{
    /* NOTE(Joey):

      Texture

    */
    class Texture
    {
    public:
        // TODO(Joey): these should be private and only accessed w/ getters/setters s.t. we can 
        // directly change the texture state where relevant from within the setters.
        GLenum Type      = GL_TEXTURE_2D;           // NOTE(Joey): what type of texture we're dealing with
        GLenum Format    = GL_RGBA;                 // NOTE(Joey): the format each texel is stored in
        GLenum FilterMin = GL_LINEAR_MIPMAP_LINEAR; // NOTE(Joey): what filter method to use during minification
        GLenum FilterMax = GL_LINEAR;               // NOTE(Joey): what filter method to use during magnification
        GLenum WrapS     = GL_REPEAT;               // NOTE(Joey): wrapping method of the S coordinate
        GLenum WrapT     = GL_REPEAT;               // NOTE(Joey): wrapping method of the T coordinate
        GLenum WrapR     = GL_REPEAT;               // NOTE(Joey): wrapping method of the R coordinate
        bool Mipmapping  = true;
    private:
        unsigned int m_ID;  
    public:
         Texture();
        ~Texture();

        // NOTE(Joey): 1D texture generation
        void Generate(unsigned int width, GLenum dataFormat, unsigned char *data);
        // NOTE(Joey): 2D texture generation
        void Generate(unsigned int width, unsigned int height, GLenum dataFormat, unsigned char *data);
        // NOTE(Joey): 3D texture generation
        void Generate(unsigned int width, unsigned int height, unsigned int depth, GLenum dataFormat, unsigned char *data);
        // TODO(Joey): Cubemap texture generation
        void Generate(unsigned int width, unsigned int height, GLenum dataFormat, unsigned char **data);

        void Bind(int unit = -1);
        void Unbind();
    };
}

#endif