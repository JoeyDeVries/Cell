#ifndef CELL_MESH_SPHERE_H
#define CELL_MESH_SPHERE_H

#include "mesh.h"

namespace Cell
{
    /* NOTE(Joey):

      Sphere

    */
    class Sphere : public Mesh
    {
    public:
        Sphere(unsigned int xSegments, unsigned int ySegments);
    };
}
#endif