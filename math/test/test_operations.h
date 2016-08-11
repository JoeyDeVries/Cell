#ifndef MATH_TEST_OPERATIONS_H
#define MATH_TEST_OPERATIONS_H

#include "../math.h"

// NOTE(Joey): test the validy of vector-vector and vector-scalar
// operations of the custom math library.
bool VectorOperation()
{
    bool success = true;

    math::vector<3, float> vec3(1.0f, 2.0f, 3.0f);
    vec3 = vec3 + 0.5f;
    vec3 = vec3 + math::vector<3, float>(1.0f);
    if (vec3.x != 1.0f + 0.5f + 1.0f) success = false;
    if (vec3.y != 2.0f + 0.5f + 1.0f) success = false;
    if (vec3.z != 3.0f + 0.5f + 1.0f) success = false;

    vec3 = vec3 + -vec3;
    if (vec3[0] != 0.0f) success = false;
    if (vec3[1] != 0.0f) success = false;
    if (vec3[2] != 0.0f) success = false;

    math::vector<2, double> vec2a({ 1.0, 2.0 });
    math::vector<2, double> vec2b({ 3.0, 4.0 });
    math::vector<2, double> vec2c = vec2a - vec2b;
    //vec2c = vec2c - { 1.0 };
    if (vec2c.x != 1.0 - 3.0) success = false;
    if (vec2c.y != 2.0 - 4.0) success = false;

    // NOTE(Joey): dot product
    math::vector<3, float> veca({1.0f, 2.0f, 3.0f});
    float result = math::dot(veca, veca);
    // NOTE(Joey): a vector dot product with itself is equal to its length squared
    float l = length(veca);
    float epsilon = 0.001f; // NOTE(Joey): due to floating point precision
    if(result - epsilon > l*l || result + epsilon < l*l) success = false;

    return success;
}

// NOTE(Joey): test the validy of matrix-matrix operations of the
// custom math library.
bool MatrixOperation()
{
    bool success = true;

    math::matrix<2, 2, float> mat01({ 1.0f, 2.0f, 3.0f, 4.0f });
    math::matrix<2, 2, float> mat02({ 5.0f, 6.0f, 7.0f, 8.0f });

    // NOTE(Joey): addition test
    math::matrix<2, 2, float> mat03 = mat01 + mat02;

    if (mat03[0][0] != 1.0f + 5.0f) success = false;
    if (mat03[0][1] != 2.0f + 6.0f) success = false;
    if (mat03[1][0] != 3.0f + 7.0f) success = false;
    if (mat03[1][1] != 4.0f + 8.0f) success = false;

    // NOTE(Joey): subtraction test
    math::matrix<2, 2, float> mat04 = mat02 - mat01;

    if (mat04[0][0] != 5.0f - 1.0f) success = false;
    if (mat04[0][1] != 6.0f - 2.0f) success = false;
    if (mat04[1][0] != 7.0f - 3.0f) success = false;
    if (mat04[1][1] != 8.0f - 4.0f) success = false;

    // NOTE(Joey): equal dimension multiplication test
    math::matrix<2, 2, float> mat05 = mat01 * mat02;

    if (mat05[0][0] != 1.0f * 5.0f + 3.0f * 6.0f) success = false;
    if (mat05[0][1] != 2.0f * 5.0f + 4.0f * 6.0f) success = false;
    if (mat05[1][0] != 1.0f * 7.0f + 3.0f * 8.0f) success = false;
    if (mat05[1][1] != 2.0f * 7.0f + 4.0f * 8.0f) success = false;

    // NOTE(Joey): unequal dimension multiplication test
    //           3x4 [ 7 10 13 16 ]
    //               [ 8 11 14 17 ]
    //               [ 9 12 15 18 ]
    // 2x3 [ 1 3 5 ] [ x  x  x  x ]
    //     [ 2 4 6 ] [ x  x  x  x ]
    //                             
    math::matrix<2, 3, float> mat06({
                                      1.0f, 2.0f, 3.0f, 
                                      4.0f, 5.0f, 6.0f 
                                    });
    math::matrix<3, 4, float> mat07({
                                      7.0f,  8.0f,  9.0f,
                                      10.0f, 11.0f, 12.0f,
                                      13.0f, 14.0f, 15.0f,
                                      16.0f, 17.0f, 18.0f
                                    }); 
    math::matrix<2, 4, float> mat08 = mat06 * mat07;

    if (mat08[0][0] != 1.0f *  7.0f + 3.0f *  8.0f + 5.0f *  9.0f) success = false;
    if (mat08[1][0] != 1.0f * 10.0f + 3.0f * 11.0f + 5.0f * 12.0f) success = false;
    if (mat08[2][0] != 1.0f * 13.0f + 3.0f * 14.0f + 5.0f * 15.0f) success = false;
    if (mat08[3][0] != 1.0f * 16.0f + 3.0f * 17.0f + 5.0f * 18.0f) success = false;

    if (mat08[0][1] != 2.0f *  7.0f + 4.0f *  8.0f + 6.0f *  9.0f) success = false;
    if (mat08[1][1] != 2.0f * 10.0f + 4.0f * 11.0f + 6.0f * 12.0f) success = false;
    if (mat08[2][1] != 2.0f * 13.0f + 4.0f * 14.0f + 6.0f * 15.0f) success = false;
    if (mat08[3][1] != 2.0f * 16.0f + 4.0f * 17.0f + 6.0f * 18.0f) success = false;

    return success;
}

bool VectorMatrixOperation()
{
    bool success = true;

    // NOTE(Joey): vector scale transformation
    math::matrix<3, 3, float> scale({
                                      2.0f, 0.0f, 0.0f,
                                      0.0f, 3.0f, 0.0f,
                                      0.0f, 0.0f, 4.0f
                                    });
    math::vector<3, float> vec01({1.0f, 1.0f, 1.0f});
    vec01 = scale * vec01;

    if (vec01.x != 1.0f * 2.0f) success = false;
    if (vec01.y != 1.0f * 3.0f) success = false;
    if (vec01.z != 1.0f * 4.0f) success = false;

    // NOTE(Joey): homogenous vector translation
    math::matrix<4, 4, float> translation;
    translation[3][0] = 1.0f;
    translation[3][1] = 2.0f;
    translation[3][2] = 3.0f;
    math::vector<4, float> vec02({0.0f, 0.0f, 0.0f, 1.0f}); // NOTE(Joey): we need a homogenous coordinate of 1.0 in order for translation to work
    vec02 = translation * vec02;

    if (vec02.x != 0.0f + 1.0f) success = false;
    if (vec02.y != 0.0f + 2.0f) success = false;
    if (vec02.z != 0.0f + 3.0f) success = false;
    if (vec02.w != 1.0f) success = false;

    return success;
}



#endif