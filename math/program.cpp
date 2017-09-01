#include <iostream>

#include "test/test_vector.h"
#include "test/test_matrix.h"
#include "test/test_operations.h"
#include "test/test_common.h"
#include "test/test_transformations.h"

// todo: check googletest for testing.

bool TEST_SUCCESS = true;
#define TEST(name) \
	if (name()) { std::cout << "|O| SUCCESS: "#name << std::endl; }                       \
	else {        std::cout << "|X|  FAILED: "#name << std::endl; TEST_SUCCESS = false; } \

int main(int argc, int *argv[])
{
	// run vector math tests
	TEST(VectorConstruct);
    TEST(VectorIndexing);

	// run matrix math tests
    TEST(MatrixConstruct);
    TEST(MatrixIndexing);

    // run matrix/vector operations math tests
    TEST(VectorOperation);
    TEST(MatrixOperation);
    TEST(VectorMatrixOperation);

	// run common math tests
    TEST(LerpFunctions);
    TEST(ClampFunctions);
    TEST(RangeFunctions);
    TEST(StepFunctions);

    // run transformations matrix/vector math tests
    TEST(MatrixTransformation);

	std::cout << std::endl;
	if (TEST_SUCCESS)
		std::cout << "|O| Tests succesfully completed." << std::endl;
	else
		std::cout << "|X| Tests did not all complete succesfully, re-validate code." << std::endl;

	int c;
	std::cin >> c;
	return 1;
}