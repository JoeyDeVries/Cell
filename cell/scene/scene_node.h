#ifndef CELL_SCENE_SCENE_NODE_H
#define CELL_SCENE_SCENE_NODE_H

#include <vector>

#include <math/math.h>

namespace Cell
{
    class Scene;
    class Mesh;
    class Material;

    /* NOTE(Joey):

      An individual scene node that links a mesh to a material
      to render the mesh with while at the same time
      maintaining a parent-child hierarchy for transform
      relations.

      Each node can have any number of children (via a linked
      list) and represents a single renderable entity in a
      larger scene where aech child transform on top of their
      parent node.

    */
    // TODO(Joey): tag each node with some unique ID. I suggest doing this with
    // a hashed string ID to maintain readability while also keeping strong
    // performance characteristics; comparing ints is much faster than strings!
    class SceneNode
    {
        friend Scene;
    public:
        // NOTE(Joey): each node contains relevant render state
        Mesh        *Mesh;
        Material    *Material;
        
        math::vec3 Position = math::vec3(0.0f);
        //math::quat Rotation; // TODO(Joey): implement quaternions in math library
        math::vec4 Rotation; // NOTE(Joey): axis-angle for now; test w/ quaternions soon!
        math::vec3 Scale    = math::vec3(1.0f);


    private:
        std::vector<SceneNode*> m_Children;
        SceneNode *m_Parent;

        // NOTE(Joey): per-node transform (w/ parent-child relationship)
        math::mat4  m_Transform;
        
        // NOTE(Joey): mark the current node's tranform as dirty if it needs to be
        // re-calculated this frame.
        bool m_Dirty;

    public:
        SceneNode();
        ~SceneNode();

        // NOTE(Joey): child management
        void AddChild(SceneNode *node);
        // void RemoveChild(); // TODO(Joey): think of proper way to unqiuely idetnfiy child nodes (w/ incrementing node ID or stringed hash ID?)
        std::vector<SceneNode*> GetChildren();
        unsigned int            GetChildCount();
        SceneNode              *GetChild(unsigned int index);
        SceneNode              *GetParent();

        // NOTE(Joey): returns the transform of the current node combined with
        // its parent(s)' transform.
        math::mat4 GetTransform();

        // NOTE(Joey): re-calculates this node and its children's
        // transform components if its parent or the node itself
        // is dirty.
        //void UpdateTransform();
    };
}
#endif