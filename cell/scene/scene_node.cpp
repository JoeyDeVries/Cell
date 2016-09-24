#include "scene_node.h"

#include <assert.h>

#include "../mesh/mesh.h"
#include "../shading/material.h"


namespace Cell
{
    // ------------------------------------------------------------------------
    SceneNode::SceneNode()
    {

    }
    // ------------------------------------------------------------------------
    SceneNode::~SceneNode()
    {
        //// NOTE(Joey): traverse the linked list and for any node pointer that
        //// is not pointing to a null pointer, de-allocate that scene node.
        //for (SceneNode *child = m_Next; child; child = child->m_Next)
        //{
        //    delete child;
        //}
        // no longer necessary as we're now storing the children in a vector
    }
    // ------------------------------------------------------------------------
    void SceneNode::AddChild(SceneNode *node)
    {
        node->m_Parent = this;
        m_Children.push_back(node);
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
    SceneNode *SceneNode::GetChild(unsigned int index)
    {
        assert(index < m_Children.size());
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