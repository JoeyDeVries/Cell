#ifndef CELL_RESOURCES_RESOURCES
#define CELL_RESOURCES_RESOURCES

#include <map>
#include <string>


//class ShaderLoader;  class Shader; 
//class TextureLoader; class Texture; 
//class MeshLoader;    class Mesh
#include "../shading/shader.h"
#include "../shading/material.h"
#include "../shading/texture.h"
#include "../mesh/mesh.h"

#include "shader_loader.h"
#include "texture_loader.h"
#include "mesh_loader.h"


namespace Cell
{
    /* NOTE(Joey):

      Resources

    */
    class Resources
    {
    private:
        // NOTE(Joey): we index all resources w/ a hashed string ID
        static std::map<unsigned int, Shader>  m_Shaders;
        static std::map<unsigned int, Texture> m_Textures;
        // TODO(Joey): we can't store meshes directly as loaded models often contain more than 1 mesh
        // so we have to store them directlry into a scene graph or intermediate structure.
        static std::map<unsigned int, Mesh>    m_Meshes;

        // loader helpers
        static ShaderLoader  m_ShaderLoader;
        static TextureLoader m_TextureLoader;
        static MeshLoader    m_MeshLoader;
    public:

    private:
        // NOTE(Joey): disallow creation of any Resources object; it's defined as a static object
        Resources(); 
    public:
        static void Init();

        static Shader&  LoadShader(std::string name, std::string vsPath, std::string fsPath);
        static Texture& LoadTexture(std::string name, std::string path, GLenum type = GL_TEXTURE_2D, GLenum format = GL_RGBA);
        static Mesh&    LoadMesh(std::string name, std::string path);
    };
}


#endif 