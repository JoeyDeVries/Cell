#ifndef CELL_LIGHTING_POINT_LIGHT_H
#define CELL_LIGHTING_POINT_LIGHT_H

#include <math/linear_algebra/vector.h>

#include "../shading/material.h"
#include "../mesh/sphere.h"

namespace Cell
{
    class PointLight
    {
    private:

    public:
        math::vec3 Position;
        math::vec3 Color;

        float Radius;
    private:

    public:
        // automatically calculates volume radius from attenuation settings
        void SetAttenuation(float maxRadius);
    };
}

#endif