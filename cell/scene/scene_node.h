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
    class SceneNode
    {
    public:
        // each node contains relevant render state
        Mesh*     Mesh;
        Material* Material;

        // bounding box 
        math::vec3 BoxMin = math::vec3(-99999.0f);
        math::vec3 BoxMax = math::vec3( 99999.0f);
    private:
        std::vector<SceneNode*> m_Children;
        SceneNode *m_Parent;

        // per-node transform (w/ parent-child relationship)
        math::mat4 m_Transform;
        math::mat4 m_PrevTransform;
        math::vec3 m_Position = math::vec3(0.0f);
        math::vec4 m_Rotation; // axis-angle for now; test w/ quaternions soon!
        math::vec3 m_Scale = math::vec3(1.0f);

        // mark the current node's tranform as dirty if it needs to be re-calculated this frame
        bool m_Dirty;

        // each node is uniquely identified by a 32-bit incrementing unsigned integer
        unsigned int m_ID;

        static unsigned int CounterID;
    public:
        SceneNode(unsigned int id);
        ~SceneNode();

        // scene node transform
        void SetPosition(math::vec3 position);
        void SetRotation(math::vec4 rotation);
        void SetScale(math::vec3 scale);
        void SetScale(float scale);
        math::vec3 GetLocalPosition();
        math::vec4 GetLocalRotation();
        math::vec3 GetLocalScale();
        math::vec3 GetWorldPosition();
        math::vec3 GetWorldScale();

        // scene graph 
        unsigned int GetID();
        void AddChild(SceneNode *node);
        void RemoveChild(unsigned int id); // TODO(Joey): think of proper way to unqiuely idetnfiy child nodes (w/ incrementing node ID or stringed hash ID?)
        std::vector<SceneNode*> GetChildren();
        unsigned int            GetChildCount();
        SceneNode              *GetChild(unsigned int id);
        SceneNode              *GetChildByIndex(unsigned int index);
        SceneNode              *GetParent();

        // returns the transform of the current node combined with its parent(s)' transform.
        math::mat4 GetTransform();
        math::mat4 GetPrevTransform();

        // re-calculates this node and its children's transform components if its parent or the 
        // node itself is dirty.
        void UpdateTransform(bool updatePrevTransform = false);
    };
}
#endif