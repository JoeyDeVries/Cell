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

    void Scene::Update()
    {
        m_Root->Update();
    }


    SceneNode* Scene::MakeScene()
    {
        SceneNode *node = new SceneNode;

        return node;
    }

    SceneNode* Scene::MakeScene(Mesh *mesh, Material *material)
    {
        SceneNode *node = new SceneNode;

        node->m_Mesh = mesh;
        node->m_Material = material;

        return node;
    }
}