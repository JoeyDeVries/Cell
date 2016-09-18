#ifndef CELL_MESH_TORUS_H
#define CELL_MESH_TORUS_H

#include "mesh.h"

namespace Cell
{
    /* NOTE(Joey):

      Torus

    */
    class Torus : public Mesh
    {
    public:
        Torus(float r1, float r2, unsigned int lod1, unsigned int lod2);
    };
}

#endif