#include "fly_camera.h"

#include <math/math.h>

#include <utility/logging/log.h>

namespace Cell
{
    FlyCamera::FlyCamera(math::vec3 position, math::vec3 forward, math::vec3 up)
    {
        Position = position; 
        Forward =  forward;
        Up      =  up;

        Yaw = -90.0f;

        m_WorldUp = Up;
        m_TargetPosition = position;
    }

    void FlyCamera::Update(float dt)
    {
        // NOTE(Joey): slowly interpolate to target position each frame given some damping factor.
        // This gives smooth camera movement that fades out the closer we are to our target.
        //Position = math::lerp(Position, m_TargetPosition, dt * Damping);
        Position = math::lerp(Position, m_TargetPosition, math::clamp01(dt * Damping));

        // NOTE(Joey): calculate the new view matrix
        View = math::lookAt(Position, Position + Forward, Up);
    }

    void FlyCamera::InputKey(float dt, CAMERA_MOVEMENT direction)
    {
        float speed = MovementSpeed * dt;
        // TODO(Joey): define += and -= operators on vectors and re-factor code below.
        if (direction      == CAMERA_FORWARD)
            m_TargetPosition = m_TargetPosition + Forward*speed;
        else if (direction == CAMERA_BACK)
            m_TargetPosition = m_TargetPosition - Forward*speed;
        else if (direction == CAMERA_LEFT)
            m_TargetPosition = m_TargetPosition - Right*speed;
        else if (direction == CAMERA_RIGHT)
            m_TargetPosition = m_TargetPosition + Right*speed;
    }

    void FlyCamera::InputMouse(float deltaX, float deltaY)
    {
        float xmovement = deltaX * MouseSensitivty;
        float ymovement = deltaY * MouseSensitivty;

        Yaw   += xmovement;
        Pitch += ymovement;

        // NOTE(Joey): prevents calculating the length of the null vector belows
        if(Yaw   == 0.0f) Yaw   = 0.01f;
        if(Pitch == 0.0f) Pitch = 0.01f;

        // NOTE(Joey): it's not allowed to move the pitch above or below 90 degrees asctime
        // the current world-up vector would break our LookAt calculation.
        if (Pitch > 89.0f)  Pitch =  89.0f;
        if (Pitch < -89.0f) Pitch = -89.0f;

        // NOTE(Joey): calculate new cartesian basis vectors from yaw/pitch pair:
        // TODO(Joey): Something's still not exactly right with these equations, as I have 
        // to correct with Yaw = -90.0f. However, I couldn't find a proper set of equations
        // that DO fix the issues, anyone who do knows the equations, I'd love your input!
        math::vec3 newForward;
        newForward.x = cos(0.0174533 * Pitch) * cos(0.0174533 * Yaw);
        newForward.y = sin(0.0174533 * Pitch);
        newForward.z = cos(0.0174533 * Pitch) * sin(0.0174533 * Yaw);
        Forward = math::normalize(newForward);
        Right   = math::normalize(math::cross(Forward, m_WorldUp));
        Up      = math::cross(Right, Forward);
    }
}