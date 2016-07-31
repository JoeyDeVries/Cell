#ifndef MATH_LINEAR_ALGEBRA_OPERATION_H
#define MATH_LINEAR_ALGEBRA_OPERATION_H

#include "vector.h"
#include "matrix.h"

namespace math
{
    // NOTE(Joey): per-vector operations
    // ---------------------------------
    // NOTE(Joey): negate operator
    // NOTE(Joey): because this is a member operator, we have to
    // define this for each specialization.
    template <u32 n, typename T>
    inline vector<n, T> vector<n, T>::operator-()
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = -e[i];
        return result;
    }
    template <typename T>
    inline vector<2, T> vector<2, T>::operator-()
    {
        return { -x, -y };
    }
    template <typename T>
    inline vector<3, T> vector<3, T>::operator-()
    {
        return{ -x, -y, -z };
    }
    template <typename T>
    inline vector<4, T> vector<4, T>::operator-()
    {
        return{ -x, -y, -z, -w };
    }

    // NOTE(Joey): addition
    template <u32 n, typename T>
    inline vector<n, T> operator+(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator+(T scalar, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator+(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + rhs[i];
        return result;
    }

    // NOTE(Joey): subtraction
    template <u32 n, typename T>
    inline vector<n, T> operator-(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] - scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator-(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] - rhs[i];
        return result;
    }

    // NOTE(Joey): multiplication
    template <u32 n, typename T>
    inline vector<n, T> operator*(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] * scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator*(T scalar, vector<n, T> lhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] * scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator*(vector<n, T> lhs, vector<n, T> rhs) // NOTE(Joey): hadamard product
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] * rhs[i];
        return result;
    }

    // NOTE(Joey): division
    template <u32 n, typename T>
    inline vector<n, T> operator/(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] / scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator/(T scalar, vector<n, T> lhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] / scalar;
        return result;
    }
    template <u32 n, typename T>
    inline vector<n, T> operator/(vector<n, T> lhs, vector<n, T> rhs) // NOTE(Joey): hadamard product
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] / rhs[i];
        return result;
    }

    // NOTE(Joey): per-matrix operations
    // ---------------------------------
    // NOTE(Joey): addition (note that we do not define matrix scalar operations
    // as they are not mathematically defined; they should be defined as 
    // operations on a matrix completely filled with the respective scalar.
    // TODO(Joey): consider using pointers/references instead of using the
    // stack for passing arguments here as the memory savings might now 
    // outweigh the cost of walking the pointers.
    template <u32 m, u32 n, typename T>
    matrix<m, n, T> operator+(matrix<m, n, T> lhs, matrix<m, n, T> rhs)
    {
        matrix<m, n, T> result;
        for (u32 col = 0; col < n; ++col)
        {
            for (u32 row = 0; row < m; ++row)
            {
                result[col][row] = lhs[col][row] + rhs[col][row];
            }
        }
        return result;
    }
    // NOTE(Joey): subtraction
    template <u32 m, u32 n, typename T>
    matrix<m, n, T> operator-(matrix<m, n, T> lhs, matrix<m, n, T> rhs)
    {
        matrix<m, n, T> result;
        for (u32 col = 0; col < n; ++col)
        {
            for (u32 row = 0; row < m; ++row)
            {
                result[col][row] = lhs[col][row] - rhs[col][row];
            }
        }
        return result;
    }
    // NOTE(Joey): multiplication
    // NOTE(Joey): note that with matrix multiplication both matrices can have
    // varying dimensions/sizes as long as they adhere to the following rule:
    // - The number of columns (n) of the LHS matrix should equal the number
    //   of rows (n) on the RHS matrix.
    // The result of the matrix multiplication is then always a matrix of 
    // dimensions m x o (LHS:rows x RHS:cols) dimensions.
    template <u32 m, u32 n, u32 o, typename T>
    matrix<m, o, T> operator*(matrix<m, n, T> lhs, matrix<n, o, T> rhs)
    {
        matrix<m, o, T> result;
        for (u32 col = 0; col < o; ++col)
        {
            for (u32 row = 0; row < m; ++row)
            {
                T value = {};
                for (u32 j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
                {
                    value += lhs[j][row] * rhs[col][j];
                }
                result[col][row] = value;
            }
        }
        return result;
    }
    
    // NOTE(Joey): multiplication with reference matrix (store directly inside provided matrix)
    template <u32 m, u32 n, u32 o, typename T>
    matrix<m, o, T>& mul(matrix <m, n, T> &result, matrix<m, n, T> lhs, matrix<n, o, T> rhs)
    {
        for (u32 col = 0; col < o; ++col)
        {
            for (u32 row = 0; row < m; ++row)
            {
                T value = {};
                for (u32 j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
                {
                    value += lhs[j][row] * rhs[col][j];
                }
                result[col][row] = value;
            }
        }
        return result;
    }

    // NOTE(Joey): vector-matrix operations
    // ------------------------------------
    // NOTE(Joey): rhs vector multiplication. We only define vector-matrix
    // multiplication with the vector on the right-side of the equation due
    // to the column-major convention.
    template <u32 m, u32 n, typename T>
    vector<m, T> operator*(matrix<m, n, T> lhs, vector<n, T> rhs)
    {
        vector<m, T> result;
        for (u32 row = 0; row < m; ++row)
        {
            T value = {};
            for (u32 j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
            {
                value += lhs[j][row] * rhs[j];
            }
            result[row] = value;
        }
        return result;
    }


    // NOTE(Joey): vector geometric operations
    // ---------------------------------------
    template <u32 n, typename T>
    inline T length(vector<n, T> vec)
    {
        T result = {};
        for(u32 i = 0; i < n; ++i)
            result += vec[i] * vec[i];
        return sqrt(result);
    }

    template <u32 n, typename T>
    inline vector<n, T>& normalize(vector<n, T> &vec)
    {
        T length = length(vec);
        for(u32 i = 0; i < n; ++i)
            vec[i] = vec[i] / length;
        return vec;
    }

    template <u32 n, typename T>
    inline T dot(vector<n, T> lhs, vector<n, T> rhs)
    {
        T result = {};
        for(u32 i = 0; i < n; ++i)
            result += lhs[i] * rhs[i];
        return result;
    }

    template<u32 n, typename T>
    inline vector<n, T> cross(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;

        // TODO(Joey): only implement functions when it's 100% clear how and why
        // they function; I know the forumla for cross product, but I first want
        // to delve into the proof of why it works as it does before implementing
        // it.

        return result;
    }
}
#endif