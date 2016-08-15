#ifndef CELL_RESOURCES_TEXTURE_LOADER
#define CELL_RESOURCES_TEXTURE_LOADER

#include <GL/glew.h>

#include <string>


namespace Cell
{
    class Texture;

    /* NOTE(Joey):

      TextureLoader

    */
    class TextureLoader
    {
    public:
        static Texture Load(std::string path, GLenum Type, GLenum internalFormat);
        // TODO(Joey): read and copy original cubemap order from GL specification
        static Texture LoadCubemap(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back);
    };
}
#endif