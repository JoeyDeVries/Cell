#ifndef MATH_TEST_VECTOR_H
#define MATH_TEST_VECTOR_H

#include "../math.h"

bool VectorConstruct()
{
	math::vector<3, float> vec01();
	math::vector<3, float> vec02(1.0f);
	math::vector<3, float> vec03({ 1.0f, 2.0f, 3.0f });
	math::vector<3, float> vec04({ 1.0f, 2.0f });
	math::vector<3, float> vec05(1.0f, 2.0f, 3.0f);
    math::vector<2, float> vec06(4.0f, 5.0f);
    math::vector<3, float> vec07(vec06, 0.0f);
    math::vector<3, float> vec08(vec07.xy, 0.0f);
    math::vector<4, float> vec09(1.0f, 0.0f, 1.0f, 1.0f);
    math::vector<3, float> vec10 = vec09.rgb;
    math::vector<3, float> vec11 = math::vector<3, float>::FORWARD;

	return true;
}

bool VectorIndexing()
{
    bool result = true;

    math::vector<3, float> vec01(1.0f, 2.0f, 3.0f);
    if (vec01[0] != 1.0f) result = false;
    if (vec01[1] != 2.0f) result = false;
    if (vec01[2] != 3.0f) result = false;

    return result;
}

#endif