#ifndef MATH_LINEAR_ALGEBRA_TRANSFORMATION_H
#define MATH_LINEAR_ALGEBRA_TRANSFORMATION_H

#include <assert.h>

#include "vector.h"
#include "matrix.h"

namespace math
{
    // NOTE(Joey): scale
    // -----------------
    template <std::size_t n, typename T>
    matrix<n, n, T> scale(vector<n, T>& scale)
    {
        matrix<n, n, T> mat;
		for (std::size_t i = 0; i < n; ++i) {
			mat[i][i] = scale[i];
		}
        return mat;
    }
    // NOTE(Joey): version w/ reference
    template <std::size_t n, typename T>
    matrix<n, n, T>& scale(matrix<n, n, T>& result, vector<n, T> scale)
    {
        // NOTE(Joey): we can do a manual operation on the matrix scale
		for (std::size_t i = 0; i < n; ++i) {
			result[i][i] *= scale[i];
		}
        return result;
    }

    // NOTE(Joey): specialization on affine matrices with matrix and scale
    // vector having different dimensions
    template <typename T>
    matrix<4, 4, T> scale(vector<3, T> scale)
    {
        matrix<4, 4, T> mat;
		for (std::size_t i = 0; i < 3; ++i) {
			mat[i][i] = scale[i];
		}
        return mat;
    }
    // NOTE(Joey): version w/ reference
    template <typename T>
    matrix<4, 4, T>& scale(matrix<4, 4, T>& result, vector<3, T>& scale)
    {
        // NOTE(Joey): we can do a manual operation on the matrix scale
		for (std::size_t i = 0; i < 3; ++i) {
			result[i][i] *= scale[i];
		}
        return result;
    }

    // NOTE(Joey): rotation
    // --------------------
    // NOTE(Joey): getting a direct rotation matrix is only allowed on 3 dimensions
    template <typename T>
    matrix<4, 4, T> rotate(const vector<3, T>& axis, const T& angle)
    {
        matrix<4, 4, T> result;

        const float c = cos(angle);
        const float s = sin(angle);
        const float t = 1.0f - c;

        result[0][0] = t*axis.x*axis.x + c;
        result[0][1] = t*axis.x*axis.y + s*axis.z;
        result[0][2] = t*axis.x*axis.z - s*axis.y;

        result[1][0] = t*axis.x*axis.y - s*axis.z;
        result[1][1] = t*axis.y*axis.y + c;
        result[1][2] = t*axis.y*axis.z + s*axis.x;

        result[2][0] = t*axis.x*axis.z + s*axis.y;
        result[2][1] = t*axis.y*axis.z - s*axis.x;
        result[2][2] = t*axis.z*axis.z + c;

        return result;
    }
   
    template <typename T>
    matrix<4, 4, T>& rotate(matrix<4, 4, T> &result, const vector<3, T>& axis, const T& angle)
    {
        matrix<4, 4, T> rot = rotate(axis, angle);
        result = result * rot;
        return result;
    }


    // NOTE(Joey): translation
    // -----------------------
    // NOTE(Joey): translations are only defined for 4-dimensional matrices/vectors
    template <typename T>
    matrix<4, 4, T> translate(const vector<3, T>& translation)
    {
        matrix<4, 4, T> mat;
        mat[3] = vector<4, T>(translation, 1.0);
        return mat;
    }

    template <typename T>
    matrix<4, 4, T>& translate(matrix<4, 4, T> &result, const vector<3, T>& translation)
    {
        matrix<4, 4, T> mat;
        mat[3] = vector<4, T>(translation, 1.0);

        result = result * mat;
        return result;
    }

    // NOTE(Joey): view-space 
    // ----------------------
    template <typename T>
    matrix<4, 4, T> lookAt(vector<3, T>& position, vector<3, T>& target, vector<3, T>& worldUp)
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
    matrix<4, 4, T> orthographic(T left, T right, T top, T bottom, T near_plane, T far_plane)
    {
        matrix<4, 4, T> result;

        result[0][0] = 2.0f / (right - left);
        
        result[1][1] = 2.0f / (top - bottom);

        result[2][2] = -2.0f / (far_plane - near_plane);

        result[3][0] = -(right + left) / (right - left);
        result[3][1] = -(top + bottom) / (top - bottom);
        result[3][2] = -(far_plane + near_plane) / (far_plane - near_plane);
        result[3][3] = 1.0f;

        return result;
    }

    template <typename T>
    matrix<4, 4, T> perspective(T fov, T aspect, T near_plane, T far_plane)
    {
        matrix<4, 4, T> result;

        float top    = near_plane * tan(fov / 2.0);
        float bottom = -top;
        float right  =  top * aspect;
        float left   = -top * aspect;

        result[0][0] = (2.0f*near_plane) / (right - left);

        result[1][1] = (2.0f*near_plane) / (top - bottom);

        //result[2][0] = (right + left) / (right - left);
        //result[2][1] = (top + bottom) / (top - bottom);
        result[2][2] = -(far_plane + near_plane) / (far_plane - near_plane); // NOTE(Joey): same as: (n + f) / (n - f)
        result[2][3] = -1.0f;

        result[3][2] = -(2.0f*near_plane*far_plane) / (far_plane - near_plane); // NOTE(Joey): same as 2nf / (n-f)
        result[3][3] = 0.0f;
        

        return result;
    }
} // namespace math
#endif