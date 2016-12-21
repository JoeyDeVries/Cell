#include "plane.h"

#include "../linear_algebra/operation.h"

namespace math
{
    plane::plane(const float a, const float b, const float c, const float d)
    {
        Normal.x = a;
        Normal.y = b;
        Normal.z = c;
        Distance = d;
    }

    plane::plane(const vec3& normal, const vec3& point)
    {
        Normal   = normal;
        Distance = -dot(normal, point);
    }
} // namespace math