#include "resources.h"

#include <utility/string_id.h>
#include <utility/logging/log.h>

namespace Cell
{
    std::map<unsigned int, Shader> Resources::m_Shaders           = std::map<unsigned int, Shader>();
    std::map<unsigned int, Texture> Resources::m_Textures         = std::map<unsigned int, Texture>();
    std::map<unsigned int, TextureCube> Resources::m_TexturesCube = std::map<unsigned int, TextureCube>();
    std::map<unsigned int, Mesh> Resources::m_Meshes              = std::map<unsigned int, Mesh>();

    // ------------------------------------------------------------------------
    // TODO(Joey): think of a proper way to call or default initalize w/ this function.
    // Perhaps a global Cell.h header file, that also hosts a global Cell::Init() function
    // to completely initalize all required Cell modules (like this here)?
    void Resources::Init()
    {
        // NOTE(Joey): initialize default assets/resources that should 
        // always be available, regardless of configuration.
        Shader defaultShader = ShaderLoader::Load("shaders/default.vs", "shaders/default.fs");
        Texture placeholderTexture;

        m_Shaders[SID("default")] = defaultShader;
        m_Textures[SID("default")] = placeholderTexture;
    }

    // ------------------------------------------------------------------------
    Shader* Resources::LoadShader(std::string name, std::string vsPath, std::string fsPath, std::vector<std::string> defines)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if shader already exists, return that handle
        if(Resources::m_Shaders.find(id) != Resources::m_Shaders.end())
            return &Resources::m_Shaders[id];

        Shader shader = ShaderLoader::Load(vsPath, fsPath, defines);
        Resources::m_Shaders[id] = shader;
        return &Resources::m_Shaders[id];
    }
    // ------------------------------------------------------------------------
    Shader* Resources::GetShader(std::string name)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if shader exists, return that handle
        if (Resources::m_Shaders.find(id) != Resources::m_Shaders.end())
        {
            return &Resources::m_Shaders[id];
        }
        else
        {
            Log::Message("Requested shader: " + name + " not found!", LOG_WARNING);
            return nullptr;
        }
    }
    // ------------------------------------------------------------------------
    Texture* Resources::LoadTexture(std::string name, std::string path, GLenum target, GLenum format)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if texture already exists, return that handle
        if (Resources::m_Textures.find(id) != Resources::m_Textures.end())
            return &Resources::m_Textures[id];

        Texture texture = TextureLoader::LoadTexture(path, target, format);
        Resources::m_Textures[id] = texture;
        return &Resources::m_Textures[id];
    }
    // ------------------------------------------------------------------------
    Texture* Resources::LoadHDR(std::string name, std::string path)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if texture already exists, return that handle
        if (Resources::m_Textures.find(id) != Resources::m_Textures.end())
            return &Resources::m_Textures[id];

        Texture texture = TextureLoader::LoadHDRTexture(path);
        Resources::m_Textures[id] = texture;
        return &Resources::m_Textures[id];
    }
    // ------------------------------------------------------------------------
    Texture* Resources::GetTexture(std::string name)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if shader exists, return that handle
        if (Resources::m_Textures.find(id) != Resources::m_Textures.end())
        {
            return &Resources::m_Textures[id];
        }
        else
        {
            Log::Message("Requested texture: " + name + " not found!", LOG_WARNING);
            return nullptr;
        }
    }
    // ------------------------------------------------------------------------
    TextureCube* Resources::LoadTextureCube(std::string name, std::string folder)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if texture already exists, return that handle
        if (Resources::m_TexturesCube.find(id) != Resources::m_TexturesCube.end())
            return &Resources::m_TexturesCube[id];

        TextureCube texture = TextureLoader::LoadTextureCube(folder);
        Resources::m_TexturesCube[id] = texture;
        return &Resources::m_TexturesCube[id];
    }
    // ------------------------------------------------------------------------
    TextureCube* Resources::GetTextureCube(std::string name)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if shader exists, return that handle
        if (Resources::m_TexturesCube.find(id) != Resources::m_TexturesCube.end())
        {
            return &Resources::m_TexturesCube[id];
        }
        else
        {
            Log::Message("Requested texture cube: " + name + " not found!", LOG_WARNING);
            return nullptr;
        }
    }
    // ------------------------------------------------------------------------
    Mesh* Resources::LoadMesh(std::string name, std::string path)
    {
        unsigned int id = SID(name);
        Mesh mesh;

        // NOTE(Joey): if mesh already exists, return that handle
        if (Resources::m_Meshes.find(id) != Resources::m_Meshes.end())
            return &Resources::m_Meshes[id];


        Resources::m_Meshes[id] = mesh;
        return &Resources::m_Meshes[id];
    }
    // ------------------------------------------------------------------------
    Mesh* Resources::GetMesh(std::string name)
    {
        unsigned int id = SID(name);

        // NOTE(Joey): if shader exists, return that handle
        if (Resources::m_Meshes.find(id) != Resources::m_Meshes.end())
        {
            return &Resources::m_Meshes[id];
        }
        else
        {
            Log::Message("Requested mesh: " + name + " not found!", LOG_WARNING);
            return nullptr;
        }
    }
}