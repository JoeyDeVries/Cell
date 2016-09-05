#ifndef MATH_LINEAR_ALGEBRA_MATRIX_H
#define MATH_LINEAR_ALGEBRA_MATRIX_H

#include <initializer_list>
#include <assert.h>

#include "vector.h"


namespace math
{    
    /* NOTE(Joey):

      Generic m by n dimensional matrix template type version supporting matrices
      of any type. The matrix type follows in functionality conventions from the
      mathematical literature. By default we will only be using floating point
      matrices, but having a generic version allows us to potentially use
      double precision matrices as well (or even integer matrices).

      The matrices are stored in column-major order, the resulting transformations
      will also assume column-major order, keeping matrix-vector multiplications
      with the matrix on the left side of the equation and representing vectors
      as column vectors (post-multiplication).

      Matrix numbering by math conventions:
      |  0  1  2  3 |
      |  4  5  6  7 |
      |  8  9 10 11 |
      | 12 13 14 15 |
      Column-major layout in memory:
      [ 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 ]
      Matrix numbering if we access column-major memory sequentially in the array:
      |  0  4  8 12 |
      |  1  5  9 13 |
      |  2  6 10 14 |
      |  3  7 11 15 |

      There is no need for matrix template specialization.

    */
    template <unsigned int m, unsigned int n, typename T>
    struct matrix
    {
        union
        {
            T e[n][m];
            struct
            {
                // NOTE(Joey): allow access on a per-column basis.
                // We do not provide support for per-row access as this is not
                // sequential in memory.
                vector<m, T> col[n];
            };
        };

        // NOTE(Joey): consturctor0: default initializes matrix to identity matrix 
        matrix()
        {
            for (unsigned int col = 0; col < n; ++col)
            {
                for (unsigned int row = 0; row < m; ++row)
                {
                    e[col][row] = (col == row) ? T(1.0) : T(0.0);
                }
            }
        }

        // NOTE(Joey): constructor1: initialize matrix with initializer list
        matrix(std::initializer_list<T> args)
        {
            assert(args.size() <= m * n);
            unsigned int cols = 0, rows = 0;
            for (auto begin = args.begin(); begin != args.end(); ++begin)
            {
                e[cols][rows++] = *begin;
                if (rows >= m)
                {
                    ++cols;
                    rows = 0;
                }
            }
        }

        // NOTE(Joey): returns a column vector, that can again be indexed with the vector subscript 
        // operator. In effect: [][] and [] indexing is possible.
        vector<m, T>& operator[](const unsigned int colIndex)
        {
            assert(colIndex >= 0 && colIndex < n);
            return col[colIndex];
        }
    };

    typedef matrix<2, 2, float>  mat2;
    typedef matrix<3, 3, float>  mat3;
    typedef matrix<4, 4, float>  mat4;
    typedef matrix<2, 2, double> dmat2;
    typedef matrix<3, 3, double> dmat3;
    typedef matrix<4, 4, double> dmat4;


    // NOTE(Joey): per-matrix operations
    // ---------------------------------
    // NOTE(Joey): addition (note that we do not define matrix scalar operations
    // as they are not mathematically defined; they should be defined as 
    // operations on a matrix completely filled with the respective scalar.
    // TODO(Joey): consider using pointers/references instead of using the
    // stack for passing arguments here as the memory savings might now 
    // outweigh the cost of walking the pointers.
    template <unsigned int m, unsigned int n, typename T>
    matrix<m, n, T> operator+(matrix<m, n, T> lhs, matrix<m, n, T> rhs)
    {
        matrix<m, n, T> result;
        for (unsigned int col = 0; col < n; ++col)
        {
            for (unsigned int row = 0; row < m; ++row)
            {
                result[col][row] = lhs[col][row] + rhs[col][row];
            }
        }
        return result;
    }
    // NOTE(Joey): subtraction
    template <unsigned int m, unsigned int n, typename T>
    matrix<m, n, T> operator-(matrix<m, n, T> lhs, matrix<m, n, T> rhs)
    {
        matrix<m, n, T> result;
        for (unsigned int col = 0; col < n; ++col)
        {
            for (unsigned int row = 0; row < m; ++row)
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
    template <unsigned int m, unsigned int n, unsigned int o, typename T>
    matrix<m, o, T> operator*(matrix<m, n, T> lhs, matrix<n, o, T> rhs)
    {
        matrix<m, o, T> result;
        for (unsigned int col = 0; col < o; ++col)
        {
            for (unsigned int row = 0; row < m; ++row)
            {
                T value = {};
                for (unsigned int j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
                {
                    value += lhs[j][row] * rhs[col][j];
                }
                result[col][row] = value;
            }
        }
        return result;
    }

    // NOTE(Joey): multiplication with reference matrix (store directly inside provided matrix)
    template <unsigned int m, unsigned int n, unsigned int o, typename T>
    matrix<m, o, T>& mul(matrix <m, n, T> &result, matrix<m, n, T> lhs, matrix<n, o, T> rhs)
    {
        for (unsigned int col = 0; col < o; ++col)
        {
            for (unsigned int row = 0; row < m; ++row)
            {
                T value = {};
                for (unsigned int j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
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
    template <unsigned int m, unsigned int n, typename T>
    vector<m, T> operator*(matrix<m, n, T> lhs, vector<n, T> rhs)
    {
        vector<m, T> result;
        for (unsigned int row = 0; row < m; ++row)
        {
            T value = {};
            for (unsigned int j = 0; j < n; ++j) // NOTE(Joey): j equals col in math notation (i = row)
            {
                value += lhs[j][row] * rhs[j];
            }
            result[row] = value;
        }
        return result;
    }
}
#endif