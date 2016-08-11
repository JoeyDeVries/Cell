#ifndef CELL_MESH_CIRCLE_H
#define CELL_MESH_CIRCLE_H

#include "mesh.h"

namespace Cell
{
    /* NOTE(Joey):

      Circle

    */
    class Circle : public Mesh
    {
    public:
        Circle(unsigned int edgeSegments, unsigned int ringSegments);
    };
}
#endif