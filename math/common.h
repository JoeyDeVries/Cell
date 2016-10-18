#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include "linear_algebra/vector.h"

namespace math
{
    // NOTE(Joey): constants


    // NOTE(Joey): interpolation
    // -------------------------
    template <typename T>
    inline T lerp(T a, T b, float t)
    {
        return (1.0 - t) * a + b * t;
    }

    template <unsigned int n, typename T>
    inline vector<n, T> lerp(vector<n, T> a, vector<n, T> b, float t)
    {
        vector<n, T> result;
        for(unsigned int i = 0; i < n; ++i)
            result[i] = lerp(a[i], b[i], t);
        return result;
    }

    // NOTE(Joey): clamp
    // -----------------
    template <typename T>
    inline T clamp(T val, T min, T max)
    {
        T result = val;       

        if      (val < min) result = min;
        else if (val > max) result = max;

        return result;
    }

    template <typename T>
    inline T clamp01(T val)
    {
        return clamp<T>(val, 0.0, 1.0);
    }

    template <unsigned int n, typename T>
    inline T clamp(vector<n, T> val, T min, T max)
    {
        vector<n, T> result;
        for(unsigned int i = 0; i < n; ++i)
            result[i] = clamp(val[i], min, max);
        return result;
    }

    template <unsigned int n, typename T>
    inline T clamp01(vector<n, T> val, T min, T max)
    {
        vector<n, T> result;
        for (unsigned int i = 0; i < n; ++i)
            result[i] = clamp01(val[i], min, max);
        return result;
    }

    // NOTE(Joey): range (conversion)
    // ------------------------------
    template <typename T> 
    inline T normalizeIntoRange(T x, T start, T end)
    {
        return (x - start) / (end - start);
    }


    // NOTE(Joey): step functions 
    // --------------------------
    template <typename T>
    inline T smoothstep(T e0, T e1, T x)
    {
        T result;
        result = clamp01((x - e0) / (e1 - e0));
        result = result * result * (3.0 - 2.0 * result);
        return result;
    }

    template <typename T>
    inline T smootherstep(T e0, T e1, T x)
    {
        T result;
        result = clamp01((x - e0) / (e1 - e0));
        result = (result*result*result) * (result * (result * 6.0 - 15.0) + 10.0);
        return result;
    }

    // NOTE(Joey):
    // -----------------------


}
#endif