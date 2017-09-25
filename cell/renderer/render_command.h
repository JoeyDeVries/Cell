#ifndef CELL_RENDERER_RENDER_COMMAND_H
#define CELL_RENDERER_RENDER_COMMAND_H

#include <math/linear_algebra/matrix.h>

namespace Cell
{
    class Mesh;
    class Material;

    /* NOTE(Joey):

      render command

    */
    struct RenderCommand
    {
        math::mat4 Transform;
        math::mat4 PrevTransform;
        Mesh*      Mesh;
        Material*  Material;
    };
}

#endif