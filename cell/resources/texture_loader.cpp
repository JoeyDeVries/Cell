#include "texture_loader.h"

#include "../shading/texture.h"
#include "../shading//texture_cube.h"

#include <utility/logging/log.h>

#include "../stb/stb_image.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Texture TextureLoader::LoadTexture(std::string path, GLenum target, GLenum internalFormat, bool srgb)
    {
        Texture texture;
        texture.Target = target;
        texture.InternalFormat = internalFormat;
        if(texture.InternalFormat == GL_RGB || texture.InternalFormat == GL_SRGB)
            texture.InternalFormat = srgb ? GL_SRGB : GL_RGB;
        if (texture.InternalFormat == GL_RGBA || texture.InternalFormat == GL_SRGB_ALPHA)
            texture.InternalFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;

        // flip textures on their y coordinate while loading
        stbi_set_flip_vertically_on_load(true);

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

            if(target == GL_TEXTURE_1D)
                texture.Generate(width, texture.InternalFormat, format, GL_UNSIGNED_BYTE, data);
            else if (target == GL_TEXTURE_2D)
                texture.Generate(width, height, texture.InternalFormat, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            Log::Message("Texture failed to load at path: " + path, LOG_WARNING);
            stbi_image_free(data);
            return texture;
        }
        texture.Width = width;
        texture.Height = height;

        return texture;
    }
    // --------------------------------------------------------------------------------------------
    Texture TextureLoader::LoadHDRTexture(std::string path)
    {
        Texture texture;
        texture.Target = GL_TEXTURE_2D;
		texture.FilterMin = GL_LINEAR;
		texture.Mipmapping = false; 

        stbi_set_flip_vertically_on_load(true);

        if (stbi_is_hdr(path.c_str()))
        {
            int width, height, nrComponents;
            float *data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum internalFormat, format;
                if (nrComponents == 3)
                {
                    internalFormat = GL_RGB32F;
                    format = GL_RGB;
                }
                else if (nrComponents == 4)
                {
                    internalFormat = GL_RGBA32F;
                    format = GL_RGBA;
                }

                texture.Generate(width, height, internalFormat, format, GL_FLOAT, data);
                stbi_image_free(data);
            }
            texture.Width = width;
            texture.Height = height;
        }
        else
        {
            Log::Message("Trying to load a HDR texture with invalid path or texture is not HDR: " + path + ".", LOG_WARNING);
        }

        return texture;
    }
    // --------------------------------------------------------------------------------------------
    TextureCube TextureLoader::LoadTextureCube(std::string top, std::string bottom, std::string left, std::string right, std::string front, std::string back)
    {
        TextureCube texture;

        // disable y flip on cubemaps
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
    // --------------------------------------------------------------------------------------------
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