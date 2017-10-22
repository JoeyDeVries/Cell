#include "line_strip.h"

#include <vector>


namespace Cell
{
    // --------------------------------------------------------------------------------------------
    LineStrip::LineStrip(float width, unsigned int segments)
    {
        float deltaX = 1.0f / segments;
        for (int i = 0; i <= segments; ++i)
        {
            // x: range from -0.5f - 0.5f | y: range from -0.5f - 0.5f
            Positions.push_back({ -0.5f + (float)i * deltaX,  0.5f * width, 0.0f });
            Positions.push_back({ -0.5f + (float)i * deltaX, -0.5f * width, 0.0f });
         
            UV.push_back({ (float)i * deltaX, 1.0f });
            UV.push_back({ (float)i * deltaX, 0.0f });
        }

        Topology = TRIANGLE_STRIP;

        Finalize();
    }
}