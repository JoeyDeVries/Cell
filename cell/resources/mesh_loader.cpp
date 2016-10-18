#include "mesh_loader.h"

#include "../scene/scene_node.h"

#include <utility/logging/log.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Cell
{
    SceneNode* MeshLoader::LoadMesh(std::string path, bool setDefaultMaterial)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Message("Assimp failed to load model at path: " + path, LOG_ERROR);   
            return nullptr;
        }

        std::string directory = path.substr(0, path.find_last_of("/"));
        return nullptr;
    }
}