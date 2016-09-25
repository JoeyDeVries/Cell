#include "camera.h"


#include <math.h>
#include <math/trigonometry/conversions.h>

namespace Cell
{
    // ------------------------------------------------------------------------
    void Camera::SetPerspective(float fov, float aspect, float near, float far)
    {
        Perspective = true;
        Projection = math::perspective(fov, aspect, near, far);
        FOV = fov;
        Aspect = aspect;
    }
    // ------------------------------------------------------------------------
    void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
    {
        Perspective = false;
        Projection = math::orthographic(left, right, top, bottom, near, far);
    }
    // ------------------------------------------------------------------------
    float Camera::FrustumHeightAtDistance(float distance)
    {
        if (Perspective)
        {
             //return 2.0f * distance * tanf(math::Deg2Rad(m_Fov * 0.5));
        }
        else
        {
            return Frustum.Top.Distance;
        }
    }
    // ------------------------------------------------------------------------
    float Camera::DistanceAtFrustumHeight(float frustumHeight)
    {
        if (Perspective)
        {
            //return frustumHeight * 0.5f / tanf(math::Deg2Rad(m_Fov * 0.5f));
        }
        else
        {
            return Frustum.Near.Distance;
        }
    }

};