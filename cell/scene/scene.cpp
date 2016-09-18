#include "scene.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"

namespace Cell
{
    Scene::Scene()
    {
        m_Root = new SceneNode();
    }

    Scene::~Scene()
    {
        // NOTE(Joey): the destructor of the root's SceneNode will automatically
        // properly de-allocate its children
        delete m_Root;
    }

    void Scene::UpdateTransforms()
    {
        //m_Root->UpdateTransform();
    }

    SceneNode* Scene::GetRootNode()
    {
        return m_Root;
    }



    SceneNode* Scene::MakeSceneNode()
    {
        SceneNode *node = new SceneNode;

        return node;
    }

    SceneNode* Scene::MakeSceneNode(Mesh *mesh, Material *material)
    {
        SceneNode *node = new SceneNode;

        node->Mesh = mesh;
        node->Material = material;

        return node;
    }
}