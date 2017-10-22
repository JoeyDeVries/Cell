#ifndef CELL_MESH_QUAD_H
#define CELL_MESH_QUAD_H

#include "mesh.h"

namespace Cell
{
    /* 

      A 2D quad that's screen-facing w/ a depth position of 0.0.

      Its main use case is priamrly intended for screen-space effects as its default NDC 
      coordinates completely fill the screen. However, this usage is not enforced and they can be 
      used for other intents and purposes.

    */
    class Quad : public Mesh
    {
    public:

        Quad(); // defaults to dimensions (1, 1)
        Quad(float width, float height);
    };
}
#endif