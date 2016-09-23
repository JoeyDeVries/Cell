#include "resources.h"

#include <utility/string_id.h>


namespace Cell
{
    std::map<unsigned int, Shader> Resources::m_Shaders   = std::map<unsigned int, Shader>();
    std::map<unsigned int, Texture> Resources::m_Textures = std::map<unsigned int, Texture>();
    std::map<unsigned int, Mesh> Resources::m_Meshes      = std::map<unsigned int, Mesh>();

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

    Shader&  Resources::LoadShader(std::string name, std::string vsPath, std::string fsPath)
    {
        Shader shader = ShaderLoader::Load(vsPath, fsPath);

        unsigned int id = SID(name);
        Resources::m_Shaders[id] = shader;
        return Resources::m_Shaders[id];
    }

    Texture& Resources::LoadTexture(std::string name, std::string path, GLenum type, GLenum format)
    {
        Texture texture = TextureLoader::LoadTexture(path, type, format);

        unsigned int id = SID(name);
        Resources::m_Textures[id] = texture;
        return Resources::m_Textures[id];
    }

    Mesh&    Resources::LoadMesh(std::string name, std::string path)
    {
        unsigned int id = SID(name);
        Mesh mesh;

        Resources::m_Meshes[id] = mesh;
        return Resources::m_Meshes[id];
    }
}