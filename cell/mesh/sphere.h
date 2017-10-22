#ifndef CELL_MESH_SPHERE_H
#define CELL_MESH_SPHERE_H

#include "mesh.h"

namespace Cell
{
    /* 

      3D unit sphere charactized by its number of horizontal (xSegments) and vertical (ySegments)
      rings.

    */
    class Sphere : public Mesh
    {
    public:
        Sphere(unsigned int xSegments, unsigned int ySegments);
    };
}
#endif