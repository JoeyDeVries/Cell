#ifndef CELL_LIGHTING_POINT_LIGHT_H
#define CELL_LIGHTING_POINT_LIGHT_H

#include <math/linear_algebra/vector.h>

namespace Cell
{
    class PointLight
    {
    public:
        math::vec3 Position;
        math::vec3 Color;

        float Radius;
    };
}

#endif