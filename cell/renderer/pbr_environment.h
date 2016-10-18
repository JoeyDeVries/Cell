#ifndef CELL_RENDERER_PBR_ENVIRONMENT_H
#define CELL_RENDERER_PBR_ENVIRONMENT_H

#include "../shading/texture_cube.h"

namespace Cell
{
    /* NOTE(Joey):

      Container object for holding all per-environment specific pre-computed PBR data.

    */
    struct PBREnvironment
    {
        TextureCube *Irradiance;
        TextureCube *Prefiltered;
    };
}

#endif