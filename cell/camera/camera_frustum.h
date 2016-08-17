#ifndef CELL_CAMERA_FRUSTUM_H
#define CELL_CAMERA_FRUSTUM_H

#include <math/math.h>

namespace Cell
{
    struct FrustumPlane
    {
        math::vec3 Normal;
        float      Distance;
    };


    /* NOTE(Joey):

      Frustum

    */
    class CameraFrustum
    {
    public:
        // NOTE(Joey): order: left, right, top, bottom, near, far
        union
        {
            FrustumPlane Planes[6];
            struct
            {
                FrustumPlane Left;
                FrustumPlane Right;
                FrustumPlane Top;
                FrustumPlane Bottom;
                FrustumPlane Near;
                FrustumPlane Far;
            };
        };

    public:
        bool Intersect(math::vec3 point);
        bool Intersect(math::vec3 point, float radius);
    };
}
#endif