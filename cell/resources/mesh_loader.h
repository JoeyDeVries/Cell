#ifndef CELL_RESOURCES_MESH_LOADER_H
#define CELL_RESOURCES_MESH_LOADER_H

#include <string>

namespace Cell
{
    class SceneNode;

    /* NOTE(Joey):

      MeshLoader

    */
    class MeshLoader
    {
    public:
        static SceneNode* LoadMesh(std::string path, bool setDefaultMaterial = true);

    };
}
#endif