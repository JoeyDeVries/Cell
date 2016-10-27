#include "scene_node.h"

#include "scene.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"

#include <assert.h>

namespace Cell
{
    // ------------------------------------------------------------------------
    SceneNode::SceneNode(unsigned int id) : m_ID(id)
    {
        
    }
    // ------------------------------------------------------------------------
    SceneNode::~SceneNode()
    {
        // NOTE(Joey): traverse the list of children and delete accordingly.
        for (unsigned int i = 0; i < m_Children.size(); ++i)
        {
            // NOTE(Joey): it should not be possible that a scene node is childed by more than one
            // parent, thus we don't need to care about deleting dangling pointers.
            delete m_Children[i];
        }
    }
    // ------------------------------------------------------------------------
    unsigned int SceneNode::GetID()
    {
        return m_ID;
    }
    // ------------------------------------------------------------------------
    void SceneNode::AddChild(SceneNode *node)
    {
        // NOTE(Joey): check if this child already has a parent that's not the root scene node. If 
        // so, first remove this scene node from its current parent. Scene nodes aren't allowed to
        // exist under multiple parents.
        if (node->m_Parent && node->m_Parent != Scene::Root)
        {
            node->m_Parent->RemoveChild(m_ID);
        }
        node->m_Parent = this;
        m_Children.push_back(node);
    }
    // ------------------------------------------------------------------------
    void SceneNode::RemoveChild(unsigned int id)
    {
        auto it = std::find(m_Children.begin(), m_Children.end(), GetChild(id));
        if(it != m_Children.end())
            m_Children.erase(it);
    }
    // ------------------------------------------------------------------------
    std::vector<SceneNode*> SceneNode::GetChildren()
    {
        return m_Children;
    }
    // ------------------------------------------------------------------------
    unsigned int SceneNode::GetChildCount()
    {
        return m_Children.size();
    }
    // ------------------------------------------------------------------------
    SceneNode *SceneNode::GetChild(unsigned int id)
    {
        for (unsigned int i = 0; i < m_Children.size(); ++i)
        {
            if(m_Children[i]->GetID() == id)
                return m_Children[i];
        }
        return nullptr;
    }
    // ------------------------------------------------------------------------
    SceneNode* SceneNode::GetChildByIndex(unsigned int index)
    {
        assert(index < GetChildCount());
        return m_Children[index];
    }
    // ------------------------------------------------------------------------
    SceneNode *SceneNode::GetParent()
    {
        return m_Parent;
    }
    // ------------------------------------------------------------------------
    // NOTE(Joey): calculates the transform from its position, scale and 
    // rotation property. As the renderer generates the final transforms only 
    // once while filling the command buffer it is okay to calculate this on 
    // the spot (no dirty flags required).
    math::mat4 SceneNode::GetTransform()
    {
        // TODO(Joey): generate transform here:
        m_Transform = math::translate(Position);
        m_Transform = math::scale(m_Transform, Scale);
        m_Transform = math::rotate(m_Transform, Rotation.xyz, Rotation.w);

        // NOTE(Joey): multiply parent transform with current transform.
        if(m_Parent)
            m_Transform = m_Parent->m_Transform * m_Transform; // NOTE(Joey): note that we don't call GetTransform() as the parent's tranform should already be calculated; THIS IS RISKY! Think of better solution!

        return m_Transform;
    }

    //void SceneNode::UpdateTransform()
    //{
    //    // NOTE(Joey): first scale, then rotate, then translation
    //    // NOTE(Joey): we only do this if the node itself or its parent
    //    // is flagged as dirty.
    //    if (m_Dirty)
    //    {
    //        m_Transform = math::mat4();

    //        // NOTE(Joey): if this node is flagged as dirty, also be sure
    //        // to update its children as they have now become invalid as 
    //        // well.
    //        for (SceneNode *child = m_Next; child; child = child->m_Next)
    //        {
    //            child->m_Dirty = true;
    //            child->UpdateTransform();
    //        }
    //    }
    //}
}