#ifndef MATH_UTILITY_COMMON_H
#define MATH_UTILITY_COMMON_H

namespace math
{
    // NOTE(Joey): interpolation
    // -------------------------
    template <typename T>
    inline T lerp(T a, T, b, float t)
    {
        return (1.0 - a) * t + b * t;
    }

    template <u32 n, typename T>
    inline vector<n, T> lerp(vector<n, T> a, vector<n, T> b, float t)
    {
        vector<n, T> result;
        for(u32 i = 0; i < n; ++i)
            result[i] = lerp(a[i], b[i], t);
        return result;
    }

    // NOTE(Joey): clamp
    // -----------------
    template <typename T>
    inline T clamp(T val, T min, T max)
    {
        T result = val;       

        if      (val <= min) result = min;
        else if (val >= max) result = max;

        return result;
    }

    template <typename T>
    inline T clamp01(T val)
    {
        return clamp(val, 0.0, 1.0);
    }

    template <u32 n, typename T>
    inline T clamp(vector<n, T> val, T min, T max)
    {
        vector<n, T> result;
        for(u32 i = 0; i < n; ++i)
            result[i] = clamp(val[i], min, max);
        return result;
    }

    template <u32 n, typename T>
    inline T clamp01(vector<n, T> val, T min, T max)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = clamp01(val[i], min, max);
        return result;
    }

    // NOTE(Joey): range (conversion)
    // ------------------------------


    // NOTE(Joey): step functions 
    // --------------------------
    template <typename T>
    inline T step(T e0, T e1, T t)
    {
        return 0.0;
    }

    template <typename T>
    inline T smoothstep(T e0, T e1, T t)
    {
        return 0.0;
    }

    template <typename T>
    inline T smootherstep(T e0, T e1, T t)
    {
        return 0.0;
    }

    // NOTE(Joey):
    // -----------------------


}
#endif