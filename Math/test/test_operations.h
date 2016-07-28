#ifndef MATH_TEST_OPERATIONS_H
#define MATH_TEST_OPERATIONS_H

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
    vec2c = vec2c - { 1.0 };
    if (vec2c.x != 1.0 - 3.0) success = false;
    if (vec2c.y != 2.0 - 4.0) success = false;

    return success;
}

bool VectorMatrixOperation()
{
    return true;
}

bool MatrixOperation()
{
    return true;
}

#endif