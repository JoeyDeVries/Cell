#ifndef CELL_SHADING_TEXTURE_H
#define CELL_SHADING_TEXTURE_H

#include "../glad/glad.h"

namespace Cell
{
    /* 

      Texture

    */
    class Texture
    {
    public:
        unsigned int ID;
        // TODO(Joey): these should be private and only accessed w/ getters/setters s.t. we can 
        // directly change the texture state where relevant from within the setters.
        GLenum Target         = GL_TEXTURE_2D;           // what type of texture we're dealing with
        GLenum InternalFormat = GL_RGBA;                 // number of color components
        GLenum Format         = GL_RGBA;                 // the format each texel is stored in
        GLenum Type           = GL_UNSIGNED_BYTE;
        GLenum FilterMin      = GL_LINEAR_MIPMAP_LINEAR; // what filter method to use during minification
        GLenum FilterMax      = GL_LINEAR;               // what filter method to use during magnification
        GLenum WrapS          = GL_REPEAT;               // wrapping method of the S coordinate
        GLenum WrapT          = GL_REPEAT;               // wrapping method of the T coordinate
        GLenum WrapR          = GL_REPEAT;               // wrapping method of the R coordinate
        bool Mipmapping       = true;

        unsigned int Width  = 0;
        unsigned int Height = 0;
        unsigned int Depth  = 0;
    private:
    public:
         Texture();
        ~Texture();

        // 1D texture generation
        void Generate(unsigned int width, GLenum internalFormat, GLenum format, GLenum type, void* data);
        // 2D texture generation
        void Generate(unsigned int width, unsigned int height, GLenum internalFormat, GLenum format, GLenum type, void* data);
        // 3D texture generation
        void Generate(unsigned int width, unsigned int height, unsigned int depth, GLenum internalFormat, GLenum format, GLenum type, void* data);

        // resizes the texture; allocates new (empty) texture memory
        void Resize(unsigned int width, unsigned int height = 0, unsigned int depth = 0);

        void Bind(int unit = -1);
        void Unbind();

        // update relevant texture state
        void SetWrapMode(GLenum wrapMode, bool bind = false);
        void SetFilterMin(GLenum filter, bool bind = false);
        void SetFilterMax(GLenum filter, bool bind = false);
    };
}

#endif