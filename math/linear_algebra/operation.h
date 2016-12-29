#ifndef MATH_LINEAR_ALGEBRA_OPERATION_H
#define MATH_LINEAR_ALGEBRA_OPERATION_H

#include "vector.h"
#include "matrix.h"

// NOTE(Nabil/htmlboss): Going to try to use the built in OpenMP to speed up Matrix operations
#include <omp.h>

namespace math
{
    // NOTE(Joey): vector geometric operations
    // ---------------------------------------
    template <std::size_t  n, typename T>
    inline T length(vector<n, T> vec)
    {
        T result = {};
        for(std::size_t i = 0; i < n; ++i)
            result += vec[i] * vec[i];
        return sqrt(result);
    }
    // NOTE(Joey): often we only care about the relative length differences 
    // between vectors and not their exact length values. Seeing as a square 
    // root can be costly, it's more efficient to compare these lengtsh without
    // the square root.
    template <std::size_t n, typename T>
    inline T lengthSquared(vector<n, T> vec)
    {
        T result = {};
        for(std::size_t  i = 0; i < n; ++i)
            result += vec[i] * vec[i];
        return result;
    }

    template <std::size_t n, typename T>
    inline float distance(vector<n, T> lhs, vector<n, T> rhs)
    {
        return length(lhs - rhs);
    }
    template <std::size_t n, typename T>
    inline float distanceSquared(vector<n, T> lhs, vector<n, T> rhs)
    {
        return lengthSquared(lhs - rhs);
    }

    template <std::size_t n, typename T>
    inline vector<n, T> normalize(vector<n, T> vec)
    {
        vector<n, T> result;
        T len = length(vec);
        for(std::size_t  i = 0; i < n; ++i)
            result[i] = vec[i] / len;
        return result;
    }

    template <std::size_t  n, typename T>
    inline T dot(vector<n, T> lhs, vector<n, T> rhs)
    {
        T result = {};
        for(std::size_t i = 0; i < n; ++i)
            result += lhs[i] * rhs[i];
        return result;
    }

    // NOTE(Joey): perpendicular is only defined as is for 2D vectors
    template<typename T>
    inline vector<2, T> perpendicular(const vector<2, T>& vec)
    {
        vector<2, T> result;
        result.x = -vec.y;
        result.y = vec.x;
        return result;
    }

    // NOTE(Joey): cross product is only defined for 3D vectors
    template<typename T>
    inline vector<3, T> cross(const vector<3, T>& lhs, const vector<3, T>& rhs)
    {
        vector<3, T> result;

        result.x = lhs.y*rhs.z - lhs.z*rhs.y;
        result.y = lhs.z*rhs.x - lhs.x*rhs.z;
        result.z = lhs.x*rhs.y - lhs.y*rhs.x;

        return result;
    }

    // NOTE(Joey): matrix algebraic operations
    // ---------------------------------------
    template <unsigned int m, unsigned int n, typename T>
    inline matrix<m, n, T> transpose(matrix<m, n, T>& mat)
    {
        matrix<n, m, T> result;

        // NOTE(Joey): note that we take the rows and cols as nxm instead of mxn this time.
        // We switched the < m and < n around in loop condition (as result matrix has 
        // reversed dimensions).

		// TODO(Nabil/htmlboss): add in a test for omp parallel
		#pragma omp parallel for
        for (std::size_t col = 0; col < m; ++col)
        {
            for (std::size_t row = 0; row < n; ++row)
            {
                result[col][row] = mat[row][col];
            }
        }
        return result;
    }

    template <std::size_t  m, std::size_t  n, typename T>
    inline matrix<m, n, T> inverse(const matrix<m, n, T>& mat)
    {
        matrix<m, n, T> result;
        // TODO(Joey): calculate determinant algebraically and retrieve inverse.
        return result;
    }
} // namespace math
#endif