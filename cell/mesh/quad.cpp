#include "quad.h"



Quad::Quad()
{
    Positions = 
    {
        -1.0f,  1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    UV = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    Indices = 
    {
        0, 1, 2, 3,
    };
    Topology = TRIANGLE_STRIP;
}

Quad::Quad(float width, float height)
{
    Positions =
    {
        -width,  height, 0.0f,
        -width, -height, 0.0f,
         width, -height, 0.0f,
         width,  height, 0.0f,
    };
    UV = {
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
    };
    Indices =
    {
        0, 1, 2, 3
    };
    Topology = TRIANGLE_STRIP;
}