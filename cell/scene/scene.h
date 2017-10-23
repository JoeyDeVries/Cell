#ifndef CELL_SCENE_SCENE_H
#define CELL_SCENE_SCENE_H

#include <vector>

namespace Cell
{
    class Mesh;
    class Material;
    class SceneNode;

    /*

      The global scene object. Always holds a root node to which other scene nodes can be attached.
      There will always be one global scene object, which can be cleared, configured at will. 

    */
    class Scene
    {
    public:
        static SceneNode* Root;
        static unsigned int CounterID;
    public:
        // clears all scene nodes currently part of the scene.
        static void Clear();
 
        // static helper function that directly builds an empty scene node. Other sub-engines can 
        // directly add children to this empty scene node (w/ identity matrix as transform).
        static SceneNode* MakeSceneNode();
        //  similar to the MakeScene function, but directly builds a node w/ attached mesh and 
        // material.
        static SceneNode* MakeSceneNode(Mesh* mesh, Material* material);
        // because the scene manages we have to copy the nodes from here.
        static SceneNode* MakeSceneNode(SceneNode* node);

        // deletes a scene node from the global scene hierarchy (together with its  children).
        static void DeleteSceneNode(SceneNode* node);
    };
}
#endif