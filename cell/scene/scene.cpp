#include "scene.h"

#include "scene_node.h"
#include "../mesh/mesh.h"
#include "../shading/material.h"

#include <stack>

namespace Cell
{
    SceneNode* Scene::Root = new SceneNode(0);
    unsigned int Scene::CounterID = 0;    
    // --------------------------------------------------------------------------------------------
    void Scene::Clear()
    {       
        Scene::DeleteSceneNode(Root);
        Scene::Root = new SceneNode(0);
    }
    // --------------------------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode()
    {
        SceneNode* node = new SceneNode(Scene::CounterID++);
        // keep a global rerefence to this scene node s.t.we can clear the scene's nodes for memory 
        // management: end of program or when switching scenes.
        Root->AddChild(node);
        return node;
    }
    // --------------------------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode(Mesh* mesh, Material* material)
    {
        SceneNode* node = new SceneNode(Scene::CounterID++);

        node->Mesh = mesh;
        node->Material = material;

        // keep a global rerefence to this scene node s.t. we can clear the scene's nodes for 
        // memory management: end of program or when switching scenes.
        Root->AddChild(node);
        return node;
    }
    // --------------------------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode(SceneNode* node)
    {
        SceneNode *newNode = new SceneNode(Scene::CounterID++);

        newNode->Mesh     = node->Mesh;
        newNode->Material = node->Material;
        newNode->BoxMin   = node->BoxMin;
        newNode->BoxMax   = node->BoxMax;

        // traverse through the list of children and add them correspondingly
        std::stack<SceneNode*> nodeStack;
        for (unsigned int i = 0; i < node->GetChildCount(); ++i)
            nodeStack.push(node->GetChildByIndex(i));
        while (!nodeStack.empty())
        {
            SceneNode* child = nodeStack.top();
            nodeStack.pop();
            // similarly, create SceneNode for each child and push to scene node memory list.
            SceneNode* newChild = new SceneNode(Scene::CounterID++);
            newChild->Mesh     = child->Mesh;
            newChild->Material = child->Material;
            newChild->BoxMin   = child->BoxMin;
            newChild->BoxMax   = child->BoxMax;
            newNode->AddChild(newChild);

            for (unsigned int i = 0; i < child->GetChildCount(); ++i)
                nodeStack.push(child->GetChildByIndex(i));
        }

        Root->AddChild(newNode);
        return newNode;
    }
    // --------------------------------------------------------------------------------------------
    void Scene::DeleteSceneNode(SceneNode *node)
    {
        if (node->GetParent())
        {
            node->GetParent()->RemoveChild(node->GetID());
        }
        // all delete logic is contained within each scene node's destructor.
        delete node;
    }
}
