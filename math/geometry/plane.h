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
		vec3 Normal;
		float Distance;

		plane(const float a, const float b, const float c, const float d);
		plane(const vec3& normal, const vec3& point);
	};
} // namespace math

#endif