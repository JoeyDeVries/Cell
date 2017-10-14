#include "mesh_loader.h"

#include "resources.h"

#include "../renderer/renderer.h"
#include "../mesh/mesh.h" 
#include "../shading/material.h"
#include "../scene/scene_node.h"
#include "../shading/texture.h"

#include <utility/logging/log.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Cell
{
    std::vector<Mesh*> MeshLoader::meshStore = std::vector<Mesh*>();
    // --------------------------------------------------------------------------------------------
    void MeshLoader::Clean()
    {
        for (unsigned int i = 0; i < MeshLoader::meshStore.size(); ++i)
        {
            delete MeshLoader::meshStore[i];
        }
    }
    // --------------------------------------------------------------------------------------------
    SceneNode* MeshLoader::LoadMesh(Renderer *renderer, std::string path, bool setDefaultMaterial)
    {
        Log::Message("Loading mesh file at: " + path + ".", LOG_INIT);

        Assimp::Importer importer;
        const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            Log::Message("Assimp failed to load model at path: " + path, LOG_ERROR);   
            return nullptr;
        }

        std::string directory = path.substr(0, path.find_last_of("/"));

        Log::Message("Succesfully loaded: " + path + ".", LOG_INIT);

        return MeshLoader::processNode(renderer, scene->mRootNode, scene, directory, setDefaultMaterial);
    }
    // --------------------------------------------------------------------------------------------
    SceneNode* MeshLoader::processNode(Renderer* renderer, aiNode* aNode, const aiScene* aScene, std::string directory, bool setDefaultMaterial)
    {
        // note that we allocate memory ourselves and pass memory responsibility to calling 
        // resource manager. The resource manager is responsible for holding the scene node 
        // pointer and deleting where appropriate.
        SceneNode* node = new SceneNode(0);

        for (unsigned int i = 0; i < aNode->mNumMeshes; ++i)
        {
            math::vec3 boxMin, boxMax;
            aiMesh*     assimpMesh = aScene->mMeshes[aNode->mMeshes[i]];
            aiMaterial* assimpMat  = aScene->mMaterials[assimpMesh->mMaterialIndex];
            Mesh*       mesh       = MeshLoader::parseMesh(assimpMesh, aScene, boxMin, boxMax);
            Material*   material   = nullptr;
            if (setDefaultMaterial)
            {
                material = MeshLoader::parseMaterial(renderer, assimpMat, aScene, directory);
            }

            // if we only have one mesh, this node itself contains the mesh/material.
            if (aNode->mNumMeshes == 1)
            {
                node->Mesh = mesh;
                if (setDefaultMaterial)
                {
                    node->Material = material;
                }
                node->BoxMin = boxMin;
                node->BoxMax = boxMax;
            }
            // otherwise, the meshes are considered on equal depth of its children
            else
            {
                SceneNode* child = new SceneNode(0);
                child->Mesh = mesh;
                child->Material = material;
                child->BoxMin = boxMin;
                child->BoxMax = boxMax;
                node->AddChild(child);
            }
        }

        // also recursively parse this node's children 
        for (unsigned int i = 0; i < aNode->mNumChildren; ++i)
        {
            node->AddChild(MeshLoader::processNode(renderer, aNode->mChildren[i], aScene, directory, setDefaultMaterial));
        }

        return node;
    }
    // --------------------------------------------------------------------------------------------
    Mesh* MeshLoader::parseMesh(aiMesh* aMesh, const aiScene* aScene, math::vec3& out_Min, math::vec3& out_Max)
    {
        std::vector<math::vec3> positions;
        std::vector<math::vec2> uv;
        std::vector<math::vec3> normals;
        std::vector<math::vec3> tangents;
        std::vector<math::vec3> bitangents;
        std::vector<unsigned int> indices;

        positions.resize(aMesh->mNumVertices);
        normals.resize(aMesh->mNumVertices);
        if (aMesh->mNumUVComponents > 0)
        {
            uv.resize(aMesh->mNumVertices);
            tangents.resize(aMesh->mNumVertices);
            bitangents.resize(aMesh->mNumVertices);
        }
        // we assume a constant of 3 vertex indices per face as we always triangulate in Assimp's
        // post-processing step; otherwise you'll want transform this to a more  flexible scheme.
        indices.resize(aMesh->mNumFaces * 3);

        // store min/max point in local coordinates for calculating approximate bounding box.
        math::vec3 pMin( 99999.0);
        math::vec3 pMax(-99999.0);
        
        for (unsigned int i = 0; i < aMesh->mNumVertices; ++i)
        {
            positions[i] = math::vec3(aMesh->mVertices[i].x, aMesh->mVertices[i].y, aMesh->mVertices[i].z);
            normals[i] = math::vec3(aMesh->mNormals[i].x, aMesh->mNormals[i].y, aMesh->mNormals[i].z);
            if (aMesh->mTextureCoords[0])
            {
                uv[i] = math::vec2(aMesh->mTextureCoords[0][i].x, aMesh->mTextureCoords[0][i].y);

            }
            if (aMesh->mTangents)
            {
                tangents[i] = math::vec3(aMesh->mTangents[i].x, aMesh->mTangents[i].y, aMesh->mTangents[i].z);
                bitangents[i] = math::vec3(aMesh->mBitangents[i].x, aMesh->mBitangents[i].y, aMesh->mBitangents[i].z);
            }
            if (positions[i].x < pMin.x) pMin.x = positions[i].x;
            if (positions[i].y < pMin.y) pMin.y = positions[i].y;
            if (positions[i].z < pMin.z) pMin.z = positions[i].z;
            if (positions[i].x > pMax.x) pMax.x = positions[i].x;
            if (positions[i].y > pMax.y) pMax.y = positions[i].y;
            if (positions[i].z > pMax.z) pMax.z = positions[i].z;
        }
        for (unsigned int f = 0; f < aMesh->mNumFaces; ++f)
        {
            // we know we're always working with triangles due to TRIANGULATE option.
            for (unsigned int i = 0; i < 3; ++i) 
            {
                indices[f * 3 + i] = aMesh->mFaces[f].mIndices[i];
            }
        }

        Mesh *mesh = new Mesh;
        mesh->Positions = positions;
        mesh->UV = uv;
        mesh->Normals = normals;
        mesh->Tangents = tangents;
        mesh->Bitangents = bitangents;
        mesh->Indices = indices;
        mesh->Topology = TRIANGLES;
        mesh->Finalize(true);

        out_Min.x = pMin.x;
        out_Min.y = pMin.y;
        out_Min.z = pMin.z;
        out_Max.x = pMax.x;
        out_Max.y = pMax.y;
        out_Max.z = pMax.z;

        // store newly generated mesh in globally stored mesh store for memory de-allocation when 
        // a clean is required.
        MeshLoader::meshStore.push_back(mesh);

        return mesh;
    }
    // --------------------------------------------------------------------------------------------
    Material *MeshLoader::parseMaterial(Renderer* renderer, aiMaterial* aMaterial, const aiScene* aScene, std::string directory)
    {
        // create a unique default material for each loaded mesh.     
        Material* material;
        // check if diffuse texture has alpha, if so: make alpha blend material; 
        aiString file;
        aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
        std::string diffPath = std::string(file.C_Str());
        bool alpha = false;
        if (diffPath.find("_alpha") != std::string::npos)
        {
            material = renderer->CreateMaterial("alpha discard");
            alpha = true;
        }
        else  // else, make default deferred material
        {
            material = renderer->CreateMaterial();
        }

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
            // we only load the first of the list of diffuse textures, we don't really care about 
            // meshes with multiple diffuse layers; same holds for other texture types.
            aiString file;
            aMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
            // we name the texture the same as the filename as to reduce naming conflicts while 
            // still only loading unique textures.
            Texture* texture = Resources::LoadTexture(fileName, fileName, GL_TEXTURE_2D, alpha ? GL_RGBA : GL_RGB, true);
            if (texture)
            {
                material->SetTexture("TexAlbedo", texture, 3);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_DISPLACEMENT) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_DISPLACEMENT, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);

            Texture* texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                material->SetTexture("TexNormal", texture, 4);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);

            Texture* texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                material->SetTexture("TexMetallic", texture, 5);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_SHININESS) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_SHININESS, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
          
            Texture* texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                material->SetTexture("TexRoughness", texture, 6);
            }
        }
        if (aMaterial->GetTextureCount(aiTextureType_AMBIENT) > 0)
        {
            aiString file;
            aMaterial->GetTexture(aiTextureType_AMBIENT, 0, &file);
            std::string fileName = MeshLoader::processPath(&file, directory);
         
            Texture* texture = Resources::LoadTexture(fileName, fileName);
            if (texture)
            {
                material->SetTexture("TexAO", texture, 7);
            }
        }     

        return material;
    }
    // --------------------------------------------------------------------------------------------
    std::string MeshLoader::processPath(aiString *aPath, std::string directory)
    {
        std::string path = std::string(aPath->C_Str());
        // parse path directly if path contains "/" indicating it is an absolute path;  otherwise 
        // parse as relative.
        if(path.find(":/") == std::string::npos || path.find(":\\") == std::string::npos)
            path = directory + "/" + path;
        return path;
    }
}