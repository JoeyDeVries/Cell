#ifndef CELL_LIGHTING_DIRECTIONAL_LIGHT_H
#define CELL_LIGHTING_DIRECTIONAL_LIGHT_H

#include <math/linear_algebra/vector.h>

namespace Cell
{
    class DirectionalLight
    {
    public:
        math::vec3 Direction;
        math::vec3 Color;
    };
}

#endif