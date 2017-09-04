#ifndef CELL_LIGHTING_DIRECTIONAL_LIGHT_H
#define CELL_LIGHTING_DIRECTIONAL_LIGHT_H

#include <math/linear_algebra/vector.h>

namespace Cell
{
    class DirectionalLight
    {
    public:
        math::vec3 Direction = math::vec3(0.0f);
        math::vec3 Color     = math::vec3(1.0f);
        float Intensity      = 1.0f;
    };
}

#endif