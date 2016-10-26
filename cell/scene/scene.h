#ifndef CELL_SCENE_SCENE_H
#define CELL_SCENE_SCENE_H

#include <vector>

namespace Cell
{
    class Mesh;
    class Material;
    class SceneNode;

    /* NOTE(Joey):

      Manages the scene's list of scene nodes.
      Each scene node is a renderable entity that groups a
      mesh together with a material and its transform.

      Each scene node can have any number of children s.t.
      when a scene node changes its transform, its children
      will change accordingly as well. This allows us to
      neatly group together multiple renderable entities
      in a logical fashion.

      The renderer accepts a scene node to render which
      can be a single node or a complete node hierachy
      that the renderer will store accordingly in its
      command buffer.

      The scene class acts as a manager for a complete
      collection of scene nodes. While scene nodes can
      act as small sub scenes, the scene objects acts as
      a complete representation of an entire coherent
      scene/environment; similar to the concept of levels
      as commonly found in videogames.

    */
    // TODO(Joey): memory management; leak! connect all scene nodes to a root node? Or maintain a 
    // list of all created top scene nodes and delete when scene gets deleted?
    class Scene
    {
    public:

    private:
        static std::vector<SceneNode*> m_SceneNodes;
    public:
        
        // TODO(Joey): do we want this? 
        static void Clear();
 
        // NOTE(Joey): static helper function that directly builds an
        // empty scene node. Other sub-engines can directly add children
        // to this empty scene node (w/ identity matrix as transform).
        static SceneNode *MakeSceneNode();
        // NOTE(Joey): similar to the MakeScene function, but directly
        // builds a node w/ attached mesh and material.
        static SceneNode *MakeSceneNode(Mesh *mesh, Material *material);
        // NOTE(Joey): because the scene manages we have to copy the nodes from here.
        // TODO(Joey): is this what we want!?
        static SceneNode *MakeSceneNode(SceneNode* node);
    };
}
#endif