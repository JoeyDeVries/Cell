#ifndef CELL_MESH_CUBE_H
#define CELL_MESH_CUBE_H

#include "mesh.h" 

namespace Cell
{
    /* 

      A 3D cube in the range [-0.5, 0.5]. Vertices are hard-coded.

    */
    class Cube : public Mesh
    {
    public:
        Cube();
    };
}
#endif