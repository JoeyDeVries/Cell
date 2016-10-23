#include "mesh_loader.h"

#include "resources.h"

#include "../renderer/renderer.h"
#include "../mesh/mesh.h" 
#include "../shading/material.h"
#include "../scene/scene.h"
#include "../scene/scene_node.h"
#include "../shading/texture.h"

#include <utility/logging/log.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Cell
{
    SceneNode* MeshLoader::LoadMesh(Renderer *renderer, std::string path, bool setDefaultMaterial)
    {
        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Message("Assimp failed to load model at path: " + path, LOG_ERROR);   
            return nullptr;
        }

        std::string directory = path.substr(0, path.find_last_of("/"));

        return MeshLoader::processNode(renderer, scene->mRootNode, scene, directory, setDefaultMaterial);
    }
    // ------------------------------------------------------------------------
    SceneNode* MeshLoader::processNode(Renderer *renderer, aiNode *aNode, const aiScene *aScene, std::string directory, bool setDefaultMaterial)
    {
        SceneNode *node = Scene::MakeSceneNode();

        for (unsigned int i = 0; i < aNode->mNumMeshes; ++i)
        {
            aiMesh     *assimpMesh = aScene->mMeshes[aNode->mMeshes[i]];
            aiMaterial *assimpMat  = aScene->mMaterials[assimpMesh->mMaterialIndex];
            Mesh       *mesh       = MeshLoader::parseMesh(assimpMesh, aScene);
            Material   *material   = nullptr;
            if (setDefaultMaterial)
            {
                material = MeshLoader::parseMaterial(renderer, assimpMat, aScene, directory);
            }

            // NOTE(Joey): if we only have one mesh, this node itself contains the mesh/material.
            if (aNode->mNumMeshes == 1)
            {
                node->Mesh = mesh;
                if (setDefaultMaterial)
                {
                    node->Material = material;
                }
            }
            // NOTE(Joey): otherwise, the meshes are considered on equal depth of its children
            else
            {
                node->AddChild(Scene::MakeSceneNode(mesh, material));
            }
        }

        // NOTE(Joey): also recursively parse this node's children 
        for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
        {
            node->AddChild(MeshLoader::processNode(renderer, aNode->mChildren[i], aScene, directory, setDefaultMaterial));
        }

        return node;
    }
    // ------------------------------------------------------------------------
    Mesh* MeshLoader::parseMesh(aiMesh *aMesh, const aiScene *aScene)
    {
        std::vector<math::vec3> positions;
        std::vector<math::vec2> uv;
        std::vector<math::vec3> normals;
        std::vector<math::vec3> tangents;
        std::vector<unsigned int> indices;

        positions.resize(aMesh->mNumVertices);
        normals.resize(aMesh->mNumVertices);
        if (aMesh->mNumUVComponents > 0)
        {
            uv.resize(aMesh->mNumVertices);
            tangents.resize(aMesh->mNumVertices);
        }
        // NOTE(Joey): we assume a constant of 3 vertex indices per face as we always triangulate
        // in Assimp's post-processing step; otherwise you'll want transform this to a more 
        // flexible scheme.
        indices.resize(aMesh->mNumFaces * 3);
        
        for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
        {
            positions[i] = math::vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, 
                                      aMesh->mVertices[i].z);
            normals[i] = math::vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y,
                                    aMesh->mNormals[i].z);
            if (aMesh->mTextureCoords[0])
            {
                uv[i] = math::vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);
               
            }
            if (aMesh->mTangents)
            {
                tangents[i] = math::vec3(aMesh->mTangents[i].x, aMesh->mTangents[i].y,
                    aMesh->mTangents[i].z);
            }
        }
        for (unsigned int f = 0; f < aMesh->mNumFaces; ++f)
        {
            // NOTE(Joey): we know we're always working with triangles due to TRIANGULATE option.
            for (unsigned int i = 0; i < 3; ++i) 
            {
                indices[f * 3 + i] = aMesh->mFaces[f].mIndices[i];
            }
        }
        // TODO(Joey): memory management!
        Mesh *mesh = new Mesh;
        mesh->Positions = positions;
        mesh->UV = uv;
        mesh->Normals = normals;
        mesh->Tangents = tangents;
        mesh->Indices = indices;
        mesh->Topology = TRIANGLES;
        mesh->Finalize(true);

        return mesh;
    }
    // ------------------------------------------------------------------------
    Material *MeshLoader::parseMaterial(Renderer *renderer, aiMaterial *aMaterial, const aiScene *aScene, std::string directory)
    {
        // TODO(Joey): pass renderer, and create default material from renderer
        Material *material = renderer->CreateMaterial();

        /* NOTE(Joey):

          About texture types:

          We use a PBR metallic/roughness workflow so the loaded models are expected to have 
          textures conform the workflow: albedo, (normal), metallic, roughness, (ao). Since Assimp
          made certain assumptions regarding possible types of loaded textures it doesn't directly
          translate to our model thus we make some assumptions as well which the 3D author has to
          comply with if he wants the mesh(es) to directly render with its specified textures:
            
            - aiTextureType_DIFFUSE:   Albedo
            - aiTextureType_NORMALS:   Normal
            - aiTextureType_SPECULAR:  metallic
            - aiTextureType_SHININESS: roughness 
            - aiTextureType_AMBIENT:   AO (ambient occlusion)
            - aiTextureType_EMISSIVE:  Emissive

        */
        if (aMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
        {
            // NOTE(Joey): we only load the first of the list of diffuse textures, we don't really
            // care about meshes with multiple diffuse layers; same holds for other texture types.
            aiString file;
            aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
            // NOTE(Joey): we name the texture the same as the filename as to reduce naming 
            // conflicts while still only loading unique textures.
            Texture *texture = Resources::LoadTexture(fileName, fileName); 
            if (texture)
            {
                //texture->SetWrapMode(GL_REPEAT, true);
                material->SetTexture("TexAlbedo", texture, 3);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);

            Texture *texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                //texture->SetWrapMode(GL_REPEAT, true);
                material->SetTexture("TexNormal", texture, 4);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);

            Texture *texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                //texture->SetWrapMode(GL_REPEAT, true);
                material->SetTexture("TexMetallic", texture, 5);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_SHININESS, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
          
            Texture *texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                //texture->SetWrapMode(GL_REPEAT, true);
                material->SetTexture("TexRoughness", texture, 6);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_AMBIENT, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
         
            Texture *texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                //texture->SetWrapMode(GL_REPEAT, true);
                material->SetTexture("TexAO", texture, 7);
            }
        }     

        return material;
    }
    // ------------------------------------------------------------------------
    std::string MeshLoader::processPath(aiString *aPath, std::string directory)
    {
        std::string path = std::string(aPath->C_Str());
        // NOTE(Joey): parse path directly if path contains "/" indicating it is an absolute path; 
        // otherwise parse as relative.
        if(path.find("/") == std::string::npos) 
            path = directory + "/" + path;
        return path;
    }
}