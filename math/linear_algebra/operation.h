#ifndef MATH_LINEAR_ALGEBRA_OPERATION_H
#define MATH_LINEAR_ALGEBRA_OPERATION_H

#include "vector.h"
#include "matrix.h"

namespace math
{


    // NOTE(Joey): vector geometric operations
    // ---------------------------------------
    template <unsigned int n, typename T>
    inline T length(vector<n, T> vec)
    {
        T result = {};
        for(unsigned int i = 0; i < n; ++i)
            result += vec[i] * vec[i];
        return sqrt(result);
    }
    // NOTE(Joey): often we only care about the relative length differences 
    // between vectors and not their exact length values. Seeing as a square 
    // root can be costly, it's more efficient to compare these lengtsh without
    // the square root.
    template <unsigned int n, typename T>
    inline T lengthSquared(vector<n, T> vec)
    {
        T result = {};
        for(unsigned int i = 0; i < n; ++i)
            result += vec[i] * vec[i];
        return result;
    }

    template <unsigned int n, typename T>
    inline float distance(vector<n, T> lhs, vector<n, T> rhs)
    {
        return length(lhs - rhs);
    }
    template <unsigned int n, typename T>
    inline float distanceSquared(vector<n, T> lhs, vector<n, T> rhs)
    {
        return lengthSquared(lhs - rhs);
    }

    template <unsigned int n, typename T>
    inline vector<n, T>& normalize(vector<n, T> &vec)
    {
        T len = length(vec);
        for(unsigned int i = 0; i < n; ++i)
            vec[i] = vec[i] / len;
        return vec;
    }

    template <unsigned int n, typename T>
    inline T dot(vector<n, T> lhs, vector<n, T> rhs)
    {
        T result = {};
        for(unsigned int i = 0; i < n; ++i)
            result += lhs[i] * rhs[i];
        return result;
    }

    // NOTE(Joey): perpendicular is only defined as is for 2D vectors
    template<typename T>
    inline vector<2, T> perpendicular(vector<2, T> vec)
    {
        vector<2, T> result;
        result.x = -vec.y;
        result.y = vec.x;
        return result;
    }

    // NOTE(Joey): cross product is only defined for 3D vectors
    template<typename T>
    inline vector<3, T> cross(vector<3, T> lhs, vector<3, T> rhs)
    {
        vector<3, T> result;

        result.x = lhs.y*rhs.z - rhs.y*lhs.z;
        result.y = lhs.z*rhs.x - rhs.z*lhs.x;
        result.z = lhs.x*rhs.y - rhs.x*lhs.y;

        return result;
    }

    // NOTE(Joey): matrix algebraic operations
    // ---------------------------------------
    template <unsigned int m, unsigned int n, typename T>
    inline matrix<m, n, T> transpose(matrix<m, n, T> mat)
    {
        matrix<n, m, T> result;

        // NOTE(Joey): note that we take the rows and cols as nxm instead of mxn this time.
        // We switched the < m and < n around in loop condition (as result matrix has 
        // reversed dimensions).
        for (unsigned int col = 0; col < m; ++col)
        {
            for (unsigned int row = 0; row < n; ++row)
            {
                result[col][row] = mat[row][col];
            }
        }

        return result;
    }

    template <unsigned int m, unsigned int n, typename T>
    inline matrix<m, n, T> inverse(matrix<m, n, T> mat)
    {
        matrix<m, n, T> result;
        // TODO(Joey): calculate determinant algebraically and retrieve inverse.
        return result;
    }
}
#endif