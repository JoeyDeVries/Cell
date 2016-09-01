#include "polar.h"

#include <math.h>

#include "../linear_algebra/operation.h"

namespace math
{
    polar::polar(float r, float theta)
    {
        this->r     = r;
        this->theta = theta;
    }

    polar::polar(vec2 cartesian)
    {
        r     = length(cartesian); // NOTE(Joey): length of vec2(x, y).
        theta = atan2(cartesian.y, cartesian.x);
    }

    vec2 polar::ToCartesian()
    {
        return vec2(r * cos(theta), r * sin(theta));
    }
}