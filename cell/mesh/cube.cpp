#include "cube.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Cube::Cube()
    {
        Positions = std::vector<math::vec3> {
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3( 0.5f,  0.5f, -0.5f), 
            math::vec3( 0.5f, -0.5f, -0.5f), 
            math::vec3( 0.5f,  0.5f, -0.5f), 
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f,  0.5f, -0.5f),

            math::vec3(-0.5f, -0.5f,  0.5f),
            math::vec3( 0.5f, -0.5f,  0.5f),
            math::vec3( 0.5f,  0.5f,  0.5f),
            math::vec3( 0.5f,  0.5f,  0.5f), 
            math::vec3(-0.5f,  0.5f,  0.5f),
            math::vec3(-0.5f, -0.5f,  0.5f),

            math::vec3(-0.5f,  0.5f,  0.5f),
            math::vec3(-0.5f,  0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3(-0.5f, -0.5f,  0.5f),
            math::vec3(-0.5f,  0.5f,  0.5f),

            math::vec3( 0.5f,  0.5f,  0.5f), 
            math::vec3( 0.5f, -0.5f, -0.5f), 
            math::vec3( 0.5f,  0.5f, -0.5f), 
            math::vec3( 0.5f, -0.5f, -0.5f), 
            math::vec3( 0.5f,  0.5f,  0.5f), 
            math::vec3( 0.5f, -0.5f,  0.5f), 

            math::vec3(-0.5f, -0.5f, -0.5f),
            math::vec3( 0.5f, -0.5f, -0.5f),
            math::vec3( 0.5f, -0.5f,  0.5f),
            math::vec3( 0.5f, -0.5f,  0.5f), 
            math::vec3(-0.5f, -0.5f,  0.5f),
            math::vec3(-0.5f, -0.5f, -0.5f),

            math::vec3(-0.5f,  0.5f, -0.5f),
            math::vec3( 0.5f,  0.5f,  0.5f), 
            math::vec3( 0.5f,  0.5f, -0.5f), 
            math::vec3( 0.5f,  0.5f,  0.5f), 
            math::vec3(-0.5f,  0.5f, -0.5f),
            math::vec3(-0.5f,  0.5f,  0.5f),
        };
        UV = std::vector<math::vec2> {
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),

            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),

            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(1.0f, 0.0f),

            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),

            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 0.0f),
            math::vec2(0.0f, 1.0f),

            math::vec2(0.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(1.0f, 1.0f),
            math::vec2(1.0f, 0.0f),
            math::vec2(0.0f, 1.0f),
            math::vec2(0.0f, 0.0f),
        };
        Normals = std::vector<math::vec3>{
            math::vec3( 0.0f,  0.0f, -1.0f),
            math::vec3( 0.0f,  0.0f, -1.0f),
            math::vec3( 0.0f,  0.0f, -1.0f),
            math::vec3( 0.0f,  0.0f, -1.0f),
            math::vec3( 0.0f,  0.0f, -1.0f),
            math::vec3( 0.0f,  0.0f, -1.0f),

            math::vec3( 0.0f,  0.0f,  1.0f),
            math::vec3( 0.0f,  0.0f,  1.0f),
            math::vec3( 0.0f,  0.0f,  1.0f),
            math::vec3( 0.0f,  0.0f,  1.0f),
            math::vec3( 0.0f,  0.0f,  1.0f),
            math::vec3( 0.0f,  0.0f,  1.0f),

            math::vec3(-1.0f,  0.0f,  0.0f),
            math::vec3(-1.0f,  0.0f,  0.0f),
            math::vec3(-1.0f,  0.0f,  0.0f),
            math::vec3(-1.0f,  0.0f,  0.0f),
            math::vec3(-1.0f,  0.0f,  0.0f),
            math::vec3(-1.0f,  0.0f,  0.0f),

            math::vec3( 1.0f,  0.0f,  0.0f),
            math::vec3( 1.0f,  0.0f,  0.0f),
            math::vec3( 1.0f,  0.0f,  0.0f),
            math::vec3( 1.0f,  0.0f,  0.0f),
            math::vec3( 1.0f,  0.0f,  0.0f),
            math::vec3( 1.0f,  0.0f,  0.0f),

            math::vec3( 0.0f, -1.0f,  0.0f),
            math::vec3( 0.0f, -1.0f,  0.0f),
            math::vec3( 0.0f, -1.0f,  0.0f),
            math::vec3( 0.0f, -1.0f,  0.0f),
            math::vec3( 0.0f, -1.0f,  0.0f),
            math::vec3( 0.0f, -1.0f,  0.0f),

            math::vec3( 0.0f,  1.0f,  0.0f),
            math::vec3( 0.0f,  1.0f,  0.0f),
            math::vec3( 0.0f,  1.0f,  0.0f),
            math::vec3( 0.0f,  1.0f,  0.0f),
            math::vec3( 0.0f,  1.0f,  0.0f),
            math::vec3( 0.0f,  1.0f,  0.0f),
        };

        Topology = TRIANGLES;
        Finalize();
    }
}