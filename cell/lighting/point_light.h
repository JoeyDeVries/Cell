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
        math::vec3 Position   = math::vec3(0.0f);
        math::vec3 Color      = math::vec3(1.0f);
        float      Intensity  = 1.0f;
        float      Radius     = 1.0f;
        bool       Visible    = true;
        bool       RenderMesh = false;
    private:

    public:
        // automatically calculates volume radius from attenuation settings
        void SetAttenuation(float maxRadius);
    };
}

#endif