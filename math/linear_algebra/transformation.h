#ifndef MATH_LINEAR_ALGEBRA_TRANSFORMATION_H
#define MATH_LINEAR_ALGEBRA_TRANSFORMATION_H

#include <assert.h>

#include "vector.h"
#include "matrix.h"

namespace math
{
    // NOTE(Joey): scale
    // -----------------
    template <unsigned int n, typename T>
    matrix<n, n, T> scale(vector<n, T> scale)
    {
        matrix<n, n, T> mat;
        for (unsigned int i = 0; i < n; ++i)
            mat[i][i] = scale[i];
        return mat;
    }
    // NOTE(Joey): version w/ reference
    template <unsigned int n, typename T>
    matrix<n, n, T>& scale(matrix<n, n, T>& result, vector<n, T> scale)
    {
        // NOTE(Joey): we can do a manual operation on the matrix scale
        for (unsigned int i = 0; i < n; ++i)
            result[i][i] *= scale[i];
        return result;
    }

    // NOTE(Joey): specialization on affine matrices with matrix and scale
    // vector having different dimensions
    template <typename T>
    matrix<4, 4, T> scale(vector<3, T> scale)
    {
        matrix<4, 4, T> mat;
        for (unsigned int i = 0; i < 3; ++i)
            mat[i][i] = scale[i];
        return mat;
    }
    // NOTE(Joey): version w/ reference
    template <typename T>
    matrix<4, 4, T>& scale(matrix<4, 4, T>& result, vector<3, T> scale)
    {
        // NOTE(Joey): we can do a manual operation on the matrix scale
        for(unsigned int i = 0; i < 3; ++i)
            result[i][i] *= scale[i];
        return result;
    }

    // NOTE(Joey): rotation
    // --------------------
    // NOTE(Joey): getting a direct rotation matrix is only allowed on 3 dimensions
    template <typename T>
    matrix<3, 3, T> rotate(vector<3, T> axis, T radians)
    {
        matrix<3, 3, T> mat;
        return mat;
    }

    // TODO(Joey): find mathematical proof of rotation matrix around any axis
    template <typename T>
    matrix<3, 3, T>& rotate(matrix<3, 3, T> &result, vector<3, T> axis, T radians)
    {
        matrix<3, 3, T> mat;

        result = mat * result;
        return result;
    }

    // TODO(Joey): think of clean API way to seperate 4 dimensional vector in 3 
    // dimensions before storing it back as 4 dimensional vector.
    template <typename T>
    matrix<4, 4, T>& rotate(matrix<4, 4, T> &result, vector<3, T> axis, T radians)
    {
        matrix<3, 3, T> mat;

        result = result * mat;
        return result;
    }

    // NOTE(Joey): translation
    // -----------------------
    // NOTE(Joey): translations are only defined for 4-dimensional matrices/vectors
    template <typename T>
    matrix<4, 4, T> translate(vector<3, T> translation)
    {
        matrix<4, 4, T> mat;
        mat[3] = vector<4, T>(translation, 1.0);
        return mat;
    }

    template <typename T>
    matrix<4, 4, T>& translate(matrix<4, 4, T> &result, vector<3, T> translation)
    {
        matrix<4, 4, T> mat;
        mat[3] = vector<4, T>(translation, 1.0);

        result = result * mat;
        return result;
    }

    // NOTE(Joey): view-space 
    // ----------------------
    template <typename T>
    matrix<4, 4, T> lookAt(vector<3, T> position, vector<3, T> target, vector<3, T> worldUp)
    {
        matrix<4, 4, T> frame;
        matrix<4, 4, T> translate;

        vec3 forward = normalize(target - position);
        vec3 right   = normalize(cross(forward, worldUp));
        vec3 up      = cross(right, forward);

        frame[0].xyz = right;
        frame[1].xyz = up;
        // NOTE(Joey): negative forward as we're looking towards negative z-axis
        frame[2].xyz = -forward; 
        // NOTE(Joey): we want the inverse of the rotation part, which is equal
        // to its transpose as the matrix's column vectors represent a 
        // orthogonal basis.
        frame = transpose(frame);

        // NOTE(Joey): the inverse of the translation matrix is just its 
        // translation vector negated.
        translate[3].xyz = -position;

        return frame*translate;
    }

    // NOTE(Joey): projection
    // ----------------------
    template <typename T>
    matrix<4, 4, T> orthographic(T left, T right, T top, T bottom, T near, T far)
    {
        matrix<4, 4, T> result;

        result[0][0] = 2.0f / (right - left);
        
        result[1][1] = 2.0f / (top - bottom);

        result[2][2] = -2.0f / (far - near);

        result[3][0] = -(right + left) / (right - left);
        result[3][1] = -(top + bottom) / (top - bottom);
        result[3][2] = -(far + near) / (far - near);
        result[3][3] = 1.0f;

        return result;
    }

    template <typename T>
    matrix<4, 4, T> perspective(T fov, T aspect, T near, T far)
    {
        matrix<4, 4, T> result;

        float top    =  near * tan(fov / 2.0);
        float bottom = -top;
        float right  =  top * aspect;
        float left   = -top * aspect;

        result[0][0] = (2.0f*near) / (right - left);

        result[1][1] = (2.0f*near) / (top - bottom);

        //result[2][0] = (right + left) / (right - left);
        //result[2][1] = (top + bottom) / (top - bottom);
        result[2][2] = -(far + near) / (far - near); // NOTE(Joey): same as: (n + f) / (n - f)
        result[2][3] = -1.0f;

        result[3][2] = -(2.0f*near*far) / (far - near); // NOTE(Joey): same as 2nf / (n-f)
        result[3][3] = 0.0f;
        

        return result;
    }
}
#endif