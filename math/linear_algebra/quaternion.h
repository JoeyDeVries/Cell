#ifndef MATH_LINEAR_ALGEBRA_QUATERNION_H
#define MATH_LINEAR_ALGEBRA_QUATERNION_H

#include "vector.h"
#include "operation.h"

#include <math.h>

namespace math
{
    /* NOTE(Joey): 

      A Quaternion is a 4-dimensional vector consisting of
      a scalar part w and a 3-dimensional semi-rotation 
      axis v: { w, v } or { w, v_x, v_y, v_z }.

      Quaternions are an efficient and precise tool for
      representing rotations in 3D space as an angle-axis
      representation with w being the angle/2 and the 
      vector v being the axis of rotation, but normalized
      and scaled by sin(angle/2).

      Note that there is no need for templates here, a 
      quaternion is always a 4-dimensional entity and as 
      we always use quaternions within the range [-1,1] 
      float precision is sufficient enough.

    */
    struct quaternion
    {
        float w;
        union
        {
            struct 
            {
                float x, y, z;
            };
            vec3 r;
        };

        // NOTE(Joey): construct a zero quaternion rotation
        quaternion() : w(0.0f), x(0.0f), y(0.0f), z(0.0f) 
        {

        }

        // NOTE(Joey): construct a quaternion explicitly
        quaternion(const float w, const float x, const float y, const float z) : w(w), x(x), y(y), z(z)
        {

        }

        // NOTE(Joey): convert from axis-angle format (in radians) to quaternion
        quaternion(const vec3& axis, const float radians)
        {
            const float halfAngle = 0.5f * radians;
            w = cos(halfAngle);
            x = axis.x * sin(halfAngle);
            y = axis.y * sin(halfAngle);
            z = axis.z * sin(halfAngle);
        }
        // NOTE(Joey): convert from axis-angle format (with w being 0.0f)
        explicit quaternion(const vec3& axis) 
        {
            w = 0.0f;
            x = axis.x;
            y = axis.y;
            z = axis.z;
        }

        vec4 ToAxisAngle();
        mat3 ToMatrix();

        quaternion operator-();
    };

    // NOTE(Joey): quaternion algebra
    // ------------------------------
    inline quaternion quaternion::operator-()
    {
        return quaternion( -w, -x, -y, -z );
    }

    inline quaternion operator+(const quaternion& lhs, const quaternion& rhs)
    {
        return quaternion(lhs.w + rhs.w, lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    inline quaternion operator*(const quaternion& lhs, const float scalar)
    {
        return quaternion(scalar*lhs.w, scalar*lhs.x, scalar*lhs.y, scalar*lhs.z);
    }
    inline quaternion operator*(const float scalar, const quaternion& rhs)
    {
        return quaternion(scalar*rhs.w, scalar*rhs.x, scalar*rhs.y, scalar*rhs.z);
    }

    // NOTE(Joey): quaternion geometry
    // -------------------------------
    inline float length(const quaternion& quat)
    {
        return sqrt(quat.w*quat.w + quat.x*quat.x + quat.y*quat.y + quat.z*quat.z);
    }
    
    inline quaternion& normalize(quaternion& quat)
    {
        const float l = length(quat);
        quat = quat * (1.0f / l);
        return quat;
    }

    inline float dot(const quaternion& lhs, const quaternion& rhs)
    {
        return lhs.w*rhs.w + lhs.x*rhs.x + lhs.y*rhs.y + lhs.z*rhs.z;
    }

    inline quaternion operator*(const quaternion& lhs, const quaternion& rhs)
    {
        vec3 v1(rhs.x, rhs.y, rhs.z);
        vec3 v2(lhs.x, lhs.y, lhs.z);

        const float w = rhs.w*lhs.w - dot(rhs, lhs);
        vec3  v = rhs.w*v2 + lhs.w*v1 + cross(v2, v1);

        return quaternion(w, v.x, v.y, v.z);
    }

    // NOTE(Joey): rotates a vector p by quaternion q. 
    // This rotation equals: q * p * q^-1. To multiply q with p we transform p 
    // to a pure quaternion (0, p) with a 0 angle and default multiplication.
    // We won't take that exact equation here; instead we use a more efficient 
    // multiplication which is the expanded equation: 
    // (2w^2 - 1)*p + 2*dot(v, p)*r _ 2*w*cross(v, p) 
    // TODO(Joey): this one is important; make sure to test extensively in unit tests!
    inline vec3 operator*(const quaternion& quat, const vec3& vec)
    {
        // TODO(Joey): assert that we're dealing with a unit vector

        const float w2 = quat.w * quat.w;

        return (2.0f*w2 - 1.0f)*vec + 2.0f*dot(quat.r, vec)*quat.r + w2*cross(quat.r, vec);
    }

    // NOTE(Joey): the quaternion is assumed to be normalized (length of 1)
    inline quaternion inverse(const quaternion& quat)
    {
        // TODO(Joey): assert that we're dealing with a unity vector
        return quaternion(quat.w, -quat.x, -quat.y, -quat.z);
    }

    // NOTE(Joey): conversions
    // -----------------------
    inline vec4 quaternion::ToAxisAngle()
    {
        vec4 result;
        
        const float angle  = 2.0f * acos(w);
        const float length = sqrt(1.0f - angle*angle);

        result.xyz = vec3(x, y, z) / length;
        result.w   = angle;

        return result;
    }

    // NOTE(Joey): as there are a lot of duplicated terms there is a more 
    // efficient way to convert a quaternion to a matrix; however, to keep the
    // code just a tiny bit more readable (for educational purposes) we perform
    // the convetional math conversion.
    // See'Essential mathematics for games and interactive applications' p.165.
    // NOTTE(Joey): this operation requires the quaternion to be normalized.
    inline mat3 quaternion::ToMatrix()
    {
        mat3 mat;

        mat[0][0] = 1.0f - 2.0f*y*y - 2.0f*z*z;
        mat[0][1] = 2.0f*x*y + 2.0f*w*z;
        mat[0][2] = 2.0f*x*z - 2.0f*w*y;

        mat[1][0] = 2.0f*x*y - 2.0f*w*z;
        mat[1][1] = 1.0f - 2.0f*x*x - 2.0f*z*z;
        mat[1][2] = 2.0f*y*z + 2.0f*w*x;
        
        mat[2][0] = 2.0f*x*z + 2.0f*w*y;
        mat[2][1] = 2.0f*y*z - 2.0f*w*x;
        mat[2][2] = 1.0f - 2.0f*x*x - 2.0f*y*y;

        return mat;
    }

} // namepace math

#endif