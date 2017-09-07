#ifndef CELL_RENDERER_PBR_CAPTURE_H
#define CELL_RENDERER_PBR_CAPTURE_H

#include "../shading/texture_cube.h"

#include <math/linear_algebra/vector.h>

namespace Cell
{
    /* NOTE(Joey):

      Container object for holding all per-environment specific pre-computed PBR data.

    */
    struct PBRCapture
    {
        TextureCube *Irradiance;
        TextureCube *Prefiltered;

        math::vec3 Position;
        float Radius;
    };
}

#endif