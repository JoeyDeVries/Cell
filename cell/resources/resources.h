#ifndef CELL_RESOURCES_RESOURCES
#define CELL_RESOURCES_RESOURCES

//class ShaderLoader;  class Shader; 
//class TextureLoader; class Texture; 
//class MeshLoader;    class Mesh
#include "../shading/shader.h"
#include "../shading/material.h"
#include "../shading/texture.h"
#include "../shading/texture_cube.h"
#include "../mesh/mesh.h"

#include <map>
#include <string>

namespace Cell
{
    class SceneNode;
    class Renderer;

    /* 

      Global resource manager. Manages and maintains all resource memory used throughout the 
      rendering application. New resources are loaded from here, and duplicate resource loads 
      are prevented. Every resource is referenced by a hashed string ID.

    */
    class Resources
    {
    private:
        // we index all resources w/ a hashed string ID
        static std::map<unsigned int, Shader>      m_Shaders;
        static std::map<unsigned int, Texture>     m_Textures;
        static std::map<unsigned int, TextureCube> m_TexturesCube;
        static std::map<unsigned int, SceneNode*>  m_Meshes;
    public:

    private:
        // disallow creation of any Resources object; it's defined as a static object
        Resources(); 
    public:
        static void Init();
        static void Clean();

        // shader resources
        static Shader*      LoadShader(std::string name, std::string vsPath, std::string fsPath, std::vector<std::string> defines = std::vector<std::string>());
        static Shader*      GetShader(std::string name);
        // texture resources
        static Texture*     LoadTexture(std::string name, std::string path, GLenum target = GL_TEXTURE_2D, GLenum format = GL_RGBA, bool srgb = false);
        static Texture*     LoadHDR(std::string name, std::string path);
        static TextureCube* LoadTextureCube(std::string name, std::string folder);
        static Texture*     GetTexture(std::string name);
        static TextureCube* GetTextureCube(std::string name);
        // mesh/scene resources
        static SceneNode*  LoadMesh(Renderer* renderer, std::string name, std::string path);
        static SceneNode*  GetMesh(std::string name);
    };
}
#endif 