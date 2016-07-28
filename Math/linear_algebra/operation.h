#ifndef CELL_OPERATION_H
#define CELL_OPERATION_H

namespace math
{
    // NOTE(Joey): per-vector operations
    // ---------------------------------
    // NOTE(Joey): negate operator
    // NOTE(Joey): because this is a member operator and not a friend, we have to
    // define this for each specialization.
    template <u32 n, typename T>
    vector<n, T> vector<n, T>::operator-()
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = -e[i];
        return result;
    }
    template <typename T>
    vector<2, T> vector<2, T>::operator-()
    {
        return { -x, -y };
    }
    template <typename T>
    vector<3, T> vector<3, T>::operator-()
    {
        return{ -x, -y, -z };
    }
    template <typename T>
    vector<4, T> vector<4, T>::operator-()
    {
        return{ -x, -y, -z, -w };
    }

    // NOTE(Joey): addition
    template <u32 n, typename T>
    vector<n, T> operator+(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <u32 n, typename T>
    vector<n, T> operator+(T scalar, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + scalar;
        return result;
    }
    template <u32 n, typename T>
    vector<n, T> operator+(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] + rhs[i];
        return result;
    }

    // NOTE(Joey): subtraction
    template <u32 n, typename T>
    vector<n, T> operator-(vector<n, T> lhs, T scalar)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] - scalar;
        return result;
    }

    template <u32 n, typename T>
    vector<n, T> operator-(vector<n, T> lhs, vector<n, T> rhs)
    {
        vector<n, T> result;
        for (u32 i = 0; i < n; ++i)
            result[i] = lhs[i] - rhs[i];
        return result;
    }



    // NOTE(Joey): per-matrix operations
    // ---------------------------------





    // NOTE(Joey): vector-matrix operations
    // ------------------------------------


}
#endif