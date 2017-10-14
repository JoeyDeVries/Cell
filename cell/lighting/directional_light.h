#ifndef CELL_LIGHTING_DIRECTIONAL_LIGHT_H
#define CELL_LIGHTING_DIRECTIONAL_LIGHT_H

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/matrix.h>
#

namespace Cell
{
    class RenderTarget;

    /*

      Light container object for any 3D directional light source. Directional light types support
      shadow casting, holding a reference to the RenderTarget (and the relevant light space view 
      projection matrix) used for its shadow map generation.

    */
    class DirectionalLight
    {
    public:
        math::vec3 Direction = math::vec3(0.0f);
        math::vec3 Color     = math::vec3(1.0f);
        float Intensity      = 1.0f;

        bool CastShadows = true;
        RenderTarget* ShadowMapRT;
        math::mat4    LightSpaceViewProjection;
    };
}

#endif