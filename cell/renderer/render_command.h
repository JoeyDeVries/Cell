#ifndef CELL_RENDERER_RENDER_COMMAND_H
#define CELL_RENDERER_RENDER_COMMAND_H

#include <math/linear_algebra/matrix.h>

namespace Cell
{
    class Mesh;
    class Material;

    /* 

      All render state required for submitting a render command.

    */
    struct RenderCommand
    {
        math::mat4 transform;
        math::mat4 prevTransform;
        Mesh*      mesh;
        Material*  material;
        math::vec3 boxMin;
        math::vec3 boxMax;
    };
}

#endif