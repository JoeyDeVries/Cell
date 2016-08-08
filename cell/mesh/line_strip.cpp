#include "line_strip.h"

#include <vector>


namespace Cell
{
    LineStrip::LineStrip(float width, unsigned int length)
    {
        unsigned int nrVertices = 2 + length * 2; // 1 extra vertex per triangle + 2 extra at start

        float deltaX = 1.0f / nrVertices;
        for (int i = 0; i < nrVertices; i += 2)
        {
            // NOTE(Joey): x: range from -0.5f - 0.5f | y: range from -0.5f - 0.5f
            Positions.push_back({ -0.5f + (float)i * deltaX,  0.5f * width, 0.0f });
            Positions.push_back({ -0.5f + (float)i * deltaX, -0.5f * width, 0.0f });
         
            UV.push_back({ (float)i * deltaX, 1.0f });
            UV.push_back({ (float)i * deltaX, 0.0f });
        }

        Topology = TRIANGLE_STRIP;

        Finalize();
    }
}