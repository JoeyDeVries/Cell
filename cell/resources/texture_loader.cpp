#include "texture_loader.h"

#include "../stb/stb_image.h"

#include <utility/logging/log.h>
#include "../shading/texture.h"

namespace Cell
{
    Texture TextureLoader::Load(std::string path, GLenum Type, GLenum internalFormat)
    {
        Texture texture;
        texture.Type = Type;
        texture.Format = internalFormat;

        // NOTE(Joey): flip textures on their y coordinate while loading
        stbi_set_flip_vertically_on_load(true);

        // TODO(Joey): load float textures? stbi_loadf
        int width, height, nrComponents;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum dataFormat;
            if(nrComponents == 1)
                dataFormat = GL_RED;
            else if (nrComponents == 3)
                dataFormat = GL_RGB;
            else if (nrComponents == 4)
                dataFormat = GL_RGBA;

            if(Type == GL_TEXTURE_1D)
                texture.Generate(width, dataFormat, GL_UNSIGNED_BYTE, data);
            else if (Type == GL_TEXTURE_2D)
                texture.Generate(width, height, dataFormat, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            Log::Message("Texture at path: " + path + " failed to load.", LOG_WARNING);
            stbi_image_free(data);
            return texture;
        }
        return texture;
    }

    // TODO(Joey): implement
    Texture TextureLoader::LoadCubemap(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back)
    {
        Texture texture;

        return texture;
    }




}