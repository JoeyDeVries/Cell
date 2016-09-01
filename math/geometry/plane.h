#ifndef MATH_GEOMETRY_PLANE_H
#define MATH_GEOMETRY_PLANE_H

#include "../linear_algebra/vector.h"

namespace math
{
    /* NOTE(Joey): 

      Plane as deducted by the generalized plane equation.
      Only defined in 3-dimensional space.

    */
    struct plane
    {
        vec3  Normal;
        float Distance;

        plane(float a, float b, float c, float d);
        plane(vec3 normal, vec3 point);
    };
};

#endif