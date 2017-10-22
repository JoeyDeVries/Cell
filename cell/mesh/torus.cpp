#include "torus.h"

#include <math.h>

#include <math/math.h>

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    // NOTE(Joey): resources:
    // http://www.paulsprojects.net/tutorials/simplebump/simplebump.html
    // http://www.gamedev.net/topic/334335-geometric-shapessolids-generation/?whichpage=1%25EF%25BF%25BD
    // http://gamedev.stackexchange.com/questions/16845/how-do-i-generate-a-torus-mesh
    Torus::Torus(float r1, float r2, unsigned int numSteps1, unsigned int numSteps2)
    {
        // we generate an additional minor ring segment as we can't directly connect to the first 
        // minor ring as the last set of vertices require unique texture coordinates.
        // TODO(Joey): can probably merge these steps within the for-loop conditions for 
        // readability (as we need to do numSteps + 1 at all the relevant locations which is 
        // confusing and prone to bugs).
        Positions.resize((numSteps1 + 1) * (numSteps2 + 1));
        Normals.resize((numSteps1 + 1) * (numSteps2 + 1));
        UV.resize((numSteps1 + 1) * (numSteps2 + 1));

        
        // first calculate all points for the major ring on the xy plane (in textbook mathematics, 
        // the z-axis is considered the up axis).
        // TODO(Joey): no need really to pre-calculate these; integrate them within the main for 
        // loops.
        std::vector<math::vec3> p(numSteps1 + 1);
        float a = 0.0f;
        float step = 2.0f * math::PI / numSteps1;
        for (int i = 0; i <= numSteps1; ++i)
        {
            float x = cos(a) * r1;
            float y = sin(a) * r1;
            p[i].x = x;
            p[i].y = y;
            p[i].z = 0.0f;
            a += step;
        }

        // generate all the vertices, UVs, Normals (and Tangents/Bitangents):
        for (int i = 0; i <= numSteps1; ++i)
        {
            // the basis vectors of the ring equal the difference  vector between the minorRing 
            // center and the donut's center position (which equals the origin (0, 0, 0)) and the 
            // positive z-axis.
            math::vec3 u = math::normalize(math::vec3(0.0f) - p[i]) * r2; // Could be p[i] also        
            math::vec3 v = math::vec3(0.0f, 0.0f, 1.0f) * r2;

            // create the vertices of each minor ring segment:
            float a = 0.0f;
            float step = 2.0f * math::PI / numSteps2;
            for (int j = 0; j <= numSteps2; ++j)
            {
                float c = cos(a);
                float s = sin(a);

                Positions[i * (numSteps2 + 1) + j] = p[i] + c * u + s * v;
                UV[i * (numSteps2 + 1) + j].x = ((float)i) / ((float)numSteps1) * math::TAU; // multiply by TAU to keep UVs symmetric along both axes.
                UV[i * (numSteps2 + 1) + j].y = ((float)j) / ((float)numSteps2);
                Normals[i * (numSteps2 + 1) + j] = math::normalize(c * u + s * v);
                a += step;
            }
        }


        // generate the indicies for a triangle topology:
        // NOTE(Joey): as taken from gamedev.net resource.
        Indices.resize(numSteps1 * numSteps2 * 6);

        int index = 0;
        for (int i = 0; i < numSteps1; ++i)
        {
            int i1 = i;
            int i2 = (i1 + 1);

            for (int j = 0; j < numSteps2; ++j)
            {
                int j1 = j;
                int j2 = (j1 + 1);

                Indices[index++] = i1 * (numSteps2 + 1) + j1;
                Indices[index++] = i1 * (numSteps2 + 1) + j2;
                Indices[index++] = i2 * (numSteps2 + 1) + j1;

                Indices[index++] = i2 * (numSteps2 + 1) + j2;
                Indices[index++] = i2 * (numSteps2 + 1) + j1;
                Indices[index++] = i1 * (numSteps2 + 1) + j2;
            }
        }

        Topology = TRIANGLES;
        Finalize();
    }
}