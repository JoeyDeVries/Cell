#include "scene.h"

#include "scene_node.h"
#include "../mesh/mesh.h"
#include "../shading/material.h"

#include <stack>

namespace Cell
{
    std::vector<SceneNode*> Scene::m_SceneNodes;
    // ------------------------------------------------------------------------
    void Scene::Clear()
    {
        // NOTE(Joey): manually traverse the stack of all scene nodes due to allowing multiple 
        // scenes nodes to (cross)-reference each other, causing deletes on dangling pointers.
        // By traversing a recursive stack of all scene nodes we can manage the nodes we've 
        // previously deleted.
        std::stack<SceneNode*> nodeStack;
        std::vector<SceneNode*> deletedNodes;
        for (unsigned int i = 0; i < m_SceneNodes.size(); ++i)
        {
            nodeStack.push(m_SceneNodes[i]);
        }
        while (!nodeStack.empty())
        {
            SceneNode *top = nodeStack.top();
            nodeStack.pop();

            for (unsigned int i = 0; i < top->GetChildCount(); ++i)
            {
                nodeStack.push(top->GetChild(i));
            }
            // NOTE(Joey): only delete if it hasn't been previously deleted
            if (std::find(deletedNodes.begin(), deletedNodes.end(), top) == deletedNodes.end()) 
            {
                delete top;
                deletedNodes.push_back(top);
            }
        }

        //for (unsigned int i = 0; i < m_SceneNodes.size(); ++i)
        //{
        //        delete m_SceneNodes[i];
        //}
    }
    // ------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode()
    {
        SceneNode *node = new SceneNode;
        // NOTE(Joey) : keep a global rerefence to this scene node s.t.we can clear the scene's
        // nodes for memory management: end of program or when switching scenes.
        m_SceneNodes.push_back(node);
        return node;
    }
    // ------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode(Mesh *mesh, Material *material)
    {
        SceneNode *node = new SceneNode;

        node->Mesh = mesh;
        node->Material = material;

        // NOTE(Joey): keep a global rerefence to this scene node s.t. we can clear the scene's
        // nodes for memory management: end of program or when switching scenes.
        m_SceneNodes.push_back(node);
        return node;
    }
    // ------------------------------------------------------------------------
    SceneNode* Scene::MakeSceneNode(SceneNode *node)
    {
        SceneNode *newNode = new SceneNode;

        newNode->Mesh     = node->Mesh;
        newNode->Material = node->Material;

        // NOTE(Joey): traverse through the list of children and add them correspondingly
        std::stack<SceneNode*> childStack;
        for (unsigned int i = 0; i < node->GetChildCount(); ++i)
            childStack.push(node->GetChild(i));
        while (!childStack.empty())
        {
            SceneNode *child = childStack.top();
            childStack.pop();
            // NOTE(Joey): similarly, create new SceneNode for each child and push to global scene
            // node memory list.
            SceneNode *newChild = new SceneNode;
            newChild->Mesh     = child->Mesh;
            newChild->Material = child->Material;
            newNode->AddChild(newChild);

            for (unsigned int i = 0; i < child->GetChildCount(); ++i)
                childStack.push(child->GetChild(i));
        }

        Scene::m_SceneNodes.push_back(newNode);
        return newNode;
    }
}