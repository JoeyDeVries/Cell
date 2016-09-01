#include "scene_node.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"

namespace Cell
{
    SceneNode::SceneNode()
    {

    }

    SceneNode::~SceneNode()
    {
        // NOTE(Joey): traverse the linked list and for any node pointer that
        // is not pointing to a null pointer, de-allocate that scene node.
        for (SceneNode *child = m_Next; child; child = child->m_Next)
        {
            delete child;
        }
    }

    void SceneNode::AddChild(SceneNode *node)
    {
        SceneNode *sibling = new SceneNode;

        sibling->m_Next = m_Next;
        m_Next = sibling;
    }

    SceneNode* SceneNode::GetChildren()
    {
        return m_Next;
    }

    math::mat4 SceneNode::GetTransform()
    {
        return m_Transform;
    }

    void SceneNode::UpdateTransform()
    {
        // NOTE(Joey): first scale, then rotate, then translation
        // NOTE(Joey): we only do this if the node itself or its parent
        // is flagged as dirty.
        if (m_Dirty)
        {
            m_Transform = math::mat4();

            // NOTE(Joey): if this node is flagged as dirty, also be sure
            // to update its children as they have now become invalid as 
            // well.
            for (SceneNode *child = m_Next; child; child = child->m_Next)
            {
                child->m_Dirty = true;
                child->UpdateTransform();
            }
        }
    }
}