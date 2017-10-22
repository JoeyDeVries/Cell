#ifndef CELL_MESH_PLANE_H
#define CELL_MESH_PLANE_H

#include "mesh.h" 

namespace Cell
{
    /* 

      A 1x1 tesselated plane made of xsegments * ysegment * 2 triangles. Underlying topology is 
      triangle strips.

      The plane's default orientation is flat on the ground or spanned alongside the x and z axis, 
      with the positive y axis being the normal.

    */
    class Plane : public Mesh
    {
    public:
        Plane(unsigned int xSegments, unsigned int ySegments);
    };
}
#endif