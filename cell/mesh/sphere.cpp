#include "sphere.h" 

#include <math.h>
#include <math/math.h>

namespace Cell
{
    // TODO(Joey): check geodesic (and icosahedron tesselation); much better texture space mapping.
    // ------------------------------------------------------------------------
    // arametric equation for a sphere F(u,v, r) = [cos(u)*sin(v)*r, cos(v), sin(u)*sin(v)*r] where 
    // u is longitude [0, 2PI] and v is lattitude [0, PI] (note the difference in their range)
    Sphere::Sphere(unsigned int xSegments, unsigned int ySegments)
    {
        for (unsigned int y = 0; y <= ySegments; ++y)
        {
            for (unsigned int x = 0; x <= xSegments; ++x)
            {
                float xSegment = (float)x / (float)xSegments;
                float ySegment = (float)y / (float)ySegments;
                float xPos = std::cos(xSegment * math::TAU) * std::sin(ySegment * math::PI); // TAU is 2PI
                float yPos = std::cos(ySegment * math::PI);
                float zPos = std::sin(xSegment * math::TAU) * std::sin(ySegment * math::PI);

                Positions.push_back(math::vec3(xPos, yPos, zPos));
                UV.push_back(math::vec2(xSegment, ySegment));
                Normals.push_back(math::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (int y = 0; y < ySegments; ++y)
        {
            for (int x = 0; x < xSegments; ++x)
            {
                Indices.push_back((y + 1) * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x + 1);

                Indices.push_back((y + 1) * (xSegments + 1) + x);
                Indices.push_back(y       * (xSegments + 1) + x + 1);
                Indices.push_back((y + 1) * (xSegments + 1) + x + 1);
            }
        }

        Topology = TRIANGLES;
        Finalize();
    }
}
