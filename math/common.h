#ifndef MATH_COMMON_H
#define MATH_COMMON_H

#include "linear_algebra/vector.h"

#include <algorithm>
#include <cmath>

#ifdef _WIN32
	#define NOMINMAX // <windows.h> has a #define max, min so we need to get rid of that so std::max() and std::min() will work
#endif

namespace math
{
    // NOTE(Joey): constants


    // NOTE(Joey): interpolation
    // -------------------------
    template <typename T>
    inline T lerp(const T& a, const T& b, const float t)
    {
        return (1.0f - t) * a + b * t;
    }

    template <std::size_t n, typename T>
    inline vector<n, T> lerp(vector<n, T>& a, vector<n, T>& b, const float t)
    {
        vector<n, T> result;
		for (std::size_t i = 0; i < n; ++i) {
			result[i] = lerp(a[i], b[i], t);
		}
        return result;
    }

    // NOTE(Joey): clamp
    // -----------------
    template <typename T>
    inline T clamp(const T& val, const T& min, const T& max)
    {
		return std::max(min, std::min(val, max)); // http://stackoverflow.com/a/9324086/2231969
    }

    template <typename T>
    inline T clamp01(const T& val)
    {
        return clamp<T>(val, 0.0f, 1.0f);
    }

    template <std::size_t n, typename T>
    inline vector<n, T> clamp(const vector<n, T>& val, const T& min, const T& max)
    {
        vector<n, T> result;
		for (std::size_t i = 0; i < n; ++i) {
			result[i] = clamp(val[i], min, max);
		}
        return result;
    }

    template <std::size_t n, typename T>
    inline vector<n, T> clamp01(const vector<n, T>& val, const T& min, const T& max)
    {
        vector<n, T> result;
		for (std::size_t i = 0; i < n; ++i) {
			result[i] = clamp01(val[i], min, max);
		}
        return result;
    }

    // NOTE(Joey): range (conversion)
    // ------------------------------
    template <typename T> 
    inline T normalizeIntoRange(const T& x, const T& start, const T& end)
    {
        return (x - start) / (end - start);
    }


    // NOTE(Joey): step functions 
    // --------------------------
    template <typename T>
    inline T smoothstep(const T& e0, const T& e1, const T& x)
    {
        T result;
        result = clamp01((x - e0) / (e1 - e0));
        result = result * result * (3.0f - 2.0f * result);
        return result;
    }

    template <typename T>
    inline T smootherstep(const T& e0, const T& e1, const T& x)
    {
        T result;
        result = clamp01((x - e0) / (e1 - e0));
        result = (result * result * result) * (result * (result * 6.0f - 15.0f) + 10.0f);
        return result;
    }

    // NOTE(Joey): numeric
    // -----------------------
    template <std::size_t n, typename T>
    vector<n, T> abs(vector<n, T>& val)
    {
        vector<n, T> result;
        for (std::size_t i = 0; i < n; ++i) {
            result[i] = (T)std::abs(val[i]);
        }
        return result;
    }

 /*    vec3 min(vec3& val, float x)
    {
        vec3 result;
        for (std::size_t i = 0; i < 3; ++i) {
            result[i] = std::min(val[i], x);
        }
        return result;
    }
    vec2 min(vec2& val, float x)
    {
        vec2 result;
        for (std::size_t i = 0; i < 2; ++i) {
            result[i] = std::min(val[i], x);
        }
        return result;
    }

    vec3 max(vec3& val, float x)
    {
        vec3 result;
        for (std::size_t i = 0; i < 3; ++i) {
            result[i] = std::max(val[i], x);
        }
        return result;
    }
    vec2 max(vec2& val, float x)
    {
        vec2 result;
        for (std::size_t i = 0; i < 2; ++i) {
            result[i] = std::max(val[i], x);
        }
        return result;
    }*/

    //template <std::size_t n, typename T>
    //inline vector<n, T> min(vector<n, T>& val, T& x)
    //{
    //    vector<n, T> result;
    //    for (std::size_t i = 0; i < n; ++i) {
    //        result[i] = (T)std::min(val[i], x);
    //    }
    //    return result;
    //}

    //template <std::size_t n, typename T>
    //inline vector<n, T> max(vector<n, T>& val, const T& x)
    //{
    //    vector<n, T> result;
    //    for (std::size_t i = 0; i < n; ++i) {
    //        result[i] = std::max(val[i], x);
    //    }
    //    return result;
    //}

} // namespace math
#endif