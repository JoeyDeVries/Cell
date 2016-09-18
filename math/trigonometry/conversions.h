#ifndef MATH_TRIGONOMETRY_CONVERSIONS_H
#define MATH_TRIGONOMETRY_CONVERSIONS_H

namespace math
{
    const float PI  = 3.14159265359f;
    const float TAU = 6.28318530717f;

    // NOTE(To self): when declaring functions in a header file, and multiple 
    // compiled objects include this header file each compilation object will
    // have this function and thus there will be multiple objs with the same
    // function which generates 'already defined in ....obj' errors. Make sure
    // these are inline s.t. this won't occur.
    inline float Deg2Rad(float degrees)
    {
        return degrees / 180.0f * PI;
    }
    inline double Deg2Rad(double degrees)
    {
        return degrees / 180.0 * PI;
    }

    inline float Rad2Deg(float radians)
    {
        return radians / PI * 180.0f;
    }
    inline double Rad2Deg(double radians)
    {
        return radians / PI * 180.0;
    }
}

#endif