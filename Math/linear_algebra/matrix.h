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
      double precision matrices as well (or even intger matrices).

      The matrices are stored in column-major order, the resulting transformations
      will also assume column-major order, keeping matrix-vector multiplications
      with the matrix on the left side of the equation.

      Matrix numbering by math conventions:
      |  0  1  2  3 |
      |  4  5  6  7 |
      |  8  9 10 11 |
      | 12 13 14 15 |
      Column-major layout in memory:
      [ 0, 4, 8, 12, 1, 5, 9, 13, 2, 6, 10, 14, 3, 7, 11, 15 ]
      Matrix numbering if we access column-major memory sequentelly in the array:
      |  0  4  8 12 |
      |  1  5  9 13 |
      |  2  6 10 14 |
      |  3  7 11 15 |

      There is no need for matrix template specialization.

    */
    template <u32 m, u32 n, typename T>
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
            for (u32 col = 0; col < n; ++col)
            {
                for (u32 row = 0; row < m; ++row)
                {
                    e[col][row] = (col == row) ? T(1.0) : T(0.0);
                }
            }
        }

        // NOTE(Joey): constructor1: initialize matrix with initializer list
        matrix(std::initializer_list<T> args)
        {
            assert(args.size() <= m * n);
            u32 cols = 0, rows = 0;
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
        vector<m, T>& operator[](const u32 colIndex)
        {
            assert(colIndex >= 0 && colIndex < n);
            return col[colIndex];
        }
    };
}
#endif