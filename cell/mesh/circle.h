#ifndef CELL_MESH_CIRCLE_H
#define CELL_MESH_CIRCLE_H

#include "mesh.h"

namespace Cell
{
    /* 

      2D unit circle mesh characterized by its number of edge segments (outer edges) and the number
      of inner ring segments.

    */
    class Circle : public Mesh
    {
    public:
        Circle(unsigned int edgeSegments, unsigned int ringSegments);
    };
}
#endif