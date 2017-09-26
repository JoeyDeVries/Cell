#include "camera_frustum.h"

#include "camera.h"

namespace Cell
{
    // ------------------------------------------------------------------------
    void CameraFrustum::Update(Camera* camera)
    {

    }
    // ------------------------------------------------------------------------
    bool CameraFrustum::Intersect(math::vec3 point)
    {
        return true;
    }
    // ------------------------------------------------------------------------
    bool CameraFrustum::Intersect(math::vec3 point, float radius)
    {
        return true;
    }
    // ------------------------------------------------------------------------
    bool CameraFrustum::Intersect(math::vec3 boxMin, math::vec3 boxMax)
    {
        return true;
    }

}