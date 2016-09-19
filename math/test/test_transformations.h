#ifndef MATH_TEST_TRANSFORMATIONS_H
#define MATH_TEST_TRANSFORMATIONS_H

#include "../math.h"

bool MatrixTransformation()
{
    bool success = true;

    math::vector<2, float> vec01({ 1.0f, 2.0f });
    math::vector<3, float> vec02({ 1.0f, 2.0f, 3.0f });
    math::vector<4, float> vec03({ 1.0f, 2.0f, 3.0f, 1.0f });

    // NOTE(Joey): scale
    math::matrix<2, 2, float> scale = math::scale(math::vector<2, float>(2.0f, 4.0f));
    math::vector<2, float>    vec04 = scale * vec01;

    scale = math::matrix<2, 2, float>();
    math::scale(scale, { 2.0f, 4.0f });
    math::vector<2, float> vec05 = scale * vec01;

    if (vec04.x != 1.0f * 2.0f) success = false;
    if (vec04.y != 2.0f * 4.0f) success = false;
    if (vec05.x != 1.0f * 2.0f) success = false;
    if (vec05.y != 2.0f * 4.0f) success = false;

    // NOTE(Joey): rotate
    // TODO(Joey): make it work with 4x4 matrices
    //math::matrix<3, 3, float> rotate = math::rotate({ 0.0f, 1.0f, 0.0f }, 6.28f);
    //math::vector<3, float>     vec06 = rotate * vec02;

    //rotate = math::matrix<3, 3, float>();
    //math::rotate(rotate, { 0.0f, 1.0f, 0.0f }, 6.28f);
    //math::vector<3, float> vec07 = rotate * vec02;

 /*   if (vec06.x != 0.0f) success = false;
    if (vec06.y != 0.0f) success = false;
    if (vec06.z != 0.0f) success = false;
    if (vec07.x != 0.0f) success = false;
    if (vec07.y != 0.0f) success = false;
    if (vec07.z != 0.0f) success = false;*/

    // NOTE(Joey): translation
    math::matrix<4, 4, float> translate = math::translate(math::vector<3, float>(4.0f, 5.0f, 6.0f));
    math::vector<4, float> vec08 = translate * vec03;

    translate = math::matrix<4, 4, float>();
    math::translate(translate, { 4.0f, 5.0f, 6.0f });
    math::vector<4, float> vec09 = translate * vec03;

    if (vec08.x != 1.0f + 4.0f) success = false;
    if (vec08.y != 2.0f + 5.0f) success = false;
    if (vec08.z != 3.0f + 6.0f) success = false;
    if (vec09.x != 1.0f + 4.0f) success = false;
    if (vec09.y != 2.0f + 5.0f) success = false;
    if (vec09.z != 3.0f + 6.0f) success = false;

    return success;
}

#endif