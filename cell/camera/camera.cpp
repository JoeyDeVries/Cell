#include "camera.h"


#include <math.h>
#include <math/trigonometry/conversions.h>

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Camera::Camera()
    {

    }
    // --------------------------------------------------------------------------------------------
    Camera::Camera(math::vec3 position, math::vec3 forward, math::vec3 up) : Position(position), Forward(forward), Up(up)
    {
        UpdateView();
    }
    // --------------------------------------------------------------------------------------------
    void Camera::Update(float dt)
    {
        Frustum.Update(this);
    }
    // --------------------------------------------------------------------------------------------
    void Camera::SetPerspective(float fov, float aspect, float near, float far)
    {
        Perspective = true;
        Projection = math::perspective(fov, aspect, near, far);
        FOV    = fov;
        Aspect = aspect;
        Near   = near;
        Far    = far;
    }
    // --------------------------------------------------------------------------------------------
    void Camera::SetOrthographic(float left, float right, float top, float bottom, float near, float far)
    {
        Perspective = false;
        Projection = math::orthographic(left, right, top, bottom, near, far);
        Near = near;
        Far  = far;
    }
    // --------------------------------------------------------------------------------------------
    void Camera::UpdateView()
    {
        View = math::lookAt(Position, Position + Forward, Up);
    }
    // --------------------------------------------------------------------------------------------
    float Camera::FrustumHeightAtDistance(float distance)
    {
        if (Perspective)
        {
             return 2.0f * distance * tanf(math::Deg2Rad(FOV * 0.5));
        }
        else
        {
            return Frustum.Top.D;
        }
    }
    // --------------------------------------------------------------------------------------------
    float Camera::DistanceAtFrustumHeight(float frustumHeight)
    {
        if (Perspective)
        {
            return frustumHeight * 0.5f / tanf(math::Deg2Rad(FOV * 0.5f));
        }
        else
        {
            return Frustum.Near.D;
        }
    }

};