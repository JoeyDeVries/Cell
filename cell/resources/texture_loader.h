#ifndef CELL_RESOURCES_TEXTURE_LOADER
#define CELL_RESOURCES_TEXTURE_LOADER

#include "../glad/glad.h"

#include <string>


namespace Cell
{
    class Texture;
    class TextureCube;

    /* 

      Manages all custom logic for loading a variety of different texture files.

    */
    class TextureLoader
    {
    public:
        static Texture LoadTexture(std::string path, GLenum target, GLenum internalFormat, bool srgb = false);
        static Texture LoadHDRTexture(std::string path);
        // TODO(Joey): read and copy original cubemap order from GL specification
        static TextureCube LoadTextureCube(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back);
        // assumes default names for cubemap faces
        static TextureCube LoadTextureCube(std::string folder);
    };
}
#endif