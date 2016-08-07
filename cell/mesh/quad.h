#ifndef CELL_MESH_QUAD_H
#define CELL_MESH_QUAD_H

#include "mesh.h"

/* NOTE(Joey):

  Quad

*/
class Quad : public Mesh
{
public:

    Quad(); // NOTE(Joey): defaults to dimensions (1, 1)
    Quad(float width, float height);
};

#endif