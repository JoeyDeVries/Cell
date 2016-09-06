#include "spherical.h"

#include <math.h>

#include "../linear_algebra/operation.h"

namespace math
{
    spherical::spherical(float rho, float phi, float theta)
    {
        this->rho = rho;
        this->phi = phi;
        this->theta = theta;
    }

    spherical::spherical(vec3 cartesian)
    {
        rho   = length(cartesian);
        phi   = atan2(length(cartesian.xy), cartesian.z);
        theta = atan2(cartesian.y, cartesian.x);
    }

    vec3 spherical::ToCartesian()
    {
        return vec3(
            rho * sin(phi) * cos(theta), // NOTE(Joey); project phi onto xy plane and then polar coordinate conversion (r*cos(theta))
            rho * sin(phi) * sin(theta), // NOTE(Joey); project phi onto xy plane and then polar coordinate conversion (r*sin(theta))
            rho * cos(phi)             
        );
    }



}