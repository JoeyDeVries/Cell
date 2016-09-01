#include "plane.h"

#include "../linear_algebra/operation.h"

namespace math
{
    plane::plane(float a, float b, float c, float d)
    {
        Normal.x = a;
        Normal.y = b;
        Normal.z = c;
        Distance = d;
    }

    plane::plane(vec3 normal, vec3 point)
    {
        Normal   = normal;
        Distance = -dot(normal, point);
    }
}