#ifndef CELL_RESOURCES_TEXTURE_LOADER
#define CELL_RESOURCES_TEXTURE_LOADER

#include <GL/glew.h>

#include <string>


namespace Cell
{
    class Texture;
    class TextureCube;

    /* NOTE(Joey):

      TextureLoader

    */
    class TextureLoader
    {
    public:
        static Texture LoadTexture(std::string path, GLenum Type, GLenum internalFormat);
        // TODO(Joey): read and copy original cubemap order from GL specification
        static TextureCube LoadCubemap(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back);
        // NOTE(Joey): assumes default names for cubemap faces
        static TextureCube LoadCubemap(std::string folder); 
    };
}
#endif