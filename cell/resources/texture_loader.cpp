#include "texture_loader.h"

#include "../shading/texture.h"
#include "../shading//texture_cube.h"

#include <utility/logging/log.h>

#include "../stb/stb_image.h"

namespace Cell
{
    // ------------------------------------------------------------------------
    Texture TextureLoader::LoadTexture(std::string path, GLenum Type, GLenum internalFormat)
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
            GLenum format;
            if(nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            if(Type == GL_TEXTURE_1D)
                texture.Generate(width, format, format, GL_UNSIGNED_BYTE, data);
            else if (Type == GL_TEXTURE_2D)
                texture.Generate(width, height, format, format, GL_UNSIGNED_BYTE, data);
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
    // ------------------------------------------------------------------------
    TextureCube TextureLoader::LoadTextureCube(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back)
    {
        TextureCube texture;

        // NOTE(Joey): disable y flip on cubemaps
        stbi_set_flip_vertically_on_load(false);

        std::vector<std::string> faces = { top, bottom, left, right, front, back };
        for (unsigned int i = 0; i < faces.size(); ++i)
        {
            int width, height, nrComponents;
            unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
            
            if (data)
            {
                GLenum format;
                if (nrComponents == 3)
                    format = GL_RGB;
                else
                    format = GL_RGBA;

                texture.GenerateFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, width, height, format, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
            }
            else
            {
                Log::Message("Cube texture at path: " + faces[i] + " failed to load.", LOG_WARNING);
                stbi_image_free(data);
                return texture;
            }
        }
        if(texture.Mipmapping)
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        return texture;
    }
    // ------------------------------------------------------------------------
    TextureCube TextureLoader::LoadTextureCube(std::string folder)
    {
        return TextureLoader::LoadTextureCube(folder + "right.jpg",
                                              folder + "left.jpg",
                                              folder + "top.jpg",
                                              folder + "bottom.jpg",
                                              folder + "front.jpg",
                                              folder + "back.jpg");
    }

}