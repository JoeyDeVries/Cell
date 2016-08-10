#ifndef MATH_TEST_MATRIX_H
#define MATH_TEST_MATRIX_H

#include "../math.h"

bool MatrixConstruct()
{
    math::matrix<4, 4, float> test01();
    math::matrix<3, 3, double> test02({ 1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0 });
    math::vector<3, double> test03 = test02.col[0]; 

    return true;
}

bool MatrixIndexing()
{
    bool success = true;

    // basic indexing
    math::matrix<4, 3, float> matrix({
        1.0f,  2.0f,  3.0f,  4.0f,
        5.0f,  6.0f,  7.0f,  8.0f,
        9.0f, 10.0f, 11.0f, 12.0f
    });
    math::vector<4, float> vector({ 1.0f, 2.0f, 3.0f, 4.0f });

    math::vector<4, float> victor = matrix[0][0];

    victor[3];

    if (matrix[0][0] !=  1.0f) success = false;
    if (matrix[0][1] !=  2.0f) success = false;
    if (matrix[0][2] !=  3.0f) success = false;
    if (matrix[0][3] !=  4.0f) success = false;
    if (matrix[1][0] !=  5.0f) success = false;
    if (matrix[1][1] !=  6.0f) success = false;
    if (matrix[1][2] !=  7.0f) success = false;
    if (matrix[1][3] !=  8.0f) success = false;
    if (matrix[2][0] !=  9.0f) success = false;
    if (matrix[2][1] != 10.0f) success = false;
    if (matrix[2][2] != 11.0f) success = false;
    if (matrix[2][3] != 12.0f) success = false;

    // per-column indexing
    math::vector<2, float> vector01({ 1.0f, 2.0f });
    math::vector<2, float> vector02({ 3.0f, 4.0f });
    math::matrix<2, 2, float> matrix22;
    matrix22[0] = vector01;
    matrix22[1] = vector02;

    if (matrix22[0][0] != 1.0f) success = false;
    if (matrix22[0][1] != 2.0f) success = false;
    if (matrix22[1].x  != 3.0f) success = false;
    if (matrix22[1].y  != 4.0f) success = false;

    return success;
}


#endif