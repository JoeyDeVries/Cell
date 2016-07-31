#include <iostream>

#include <Math/math.h>
#include <Cell/ProjectLinkTest.h>

int main(int argc, int *argv[])
{



    math::mat3 test;
    math::vec3 test2;

    // NOTE(Joey): check if linking cell static library worked properly
    int linktest = SuperCalcFunc(1337);
    std::cout << linktest << std::endl;

    // NOTE(Joey): wait for keypress
    int input;
    std::cin >> input;

    return 1;
}