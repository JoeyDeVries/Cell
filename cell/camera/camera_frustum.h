#ifndef CELL_CAMERA_FRUSTUM_H
#define CELL_CAMERA_FRUSTUM_H

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/operation.h>

namespace Cell
{
    class Camera;

    struct FrustumPlane
    {
        math::vec3 Normal;
        float      D;

        void SetNormalD(math::vec3 normal, math::vec3 point)
        {
            Normal = math::normalize(normal);
            D      = -math::dot(Normal, point);
        }

        float Distance(math::vec3 point)
        {
            return math::dot(Normal, point) + D;
        }
    };


    /* NOTE(Joey):

      Frustum

    */
    class CameraFrustum
    {
    public:
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
        CameraFrustum() { } // NOTE(Joey): why do I need to define a constructor here? (otherwise I get deleted constructor error) LOOK IT UP!

        void Update(Camera* camera);

        bool Intersect(math::vec3 point);
        bool Intersect(math::vec3 point, float radius);
        bool Intersect(math::vec3 boxMin, math::vec3 boxMax);
    };
}
#endif