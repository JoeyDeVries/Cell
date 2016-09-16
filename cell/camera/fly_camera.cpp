#include "fly_camera.h"

#include <math/math.h>

#include <utility/logging/log.h>

namespace Cell
{
    FlyCamera::FlyCamera(math::vec3 position, math::vec3 forward, math::vec3 up)
    {
        // NOTE(Joey): note that the camera position is reversed from the 
        // lookAt matrix so we negate the given position. This way the user can
        // supply a position that makes sense according to world-space.
        Position = -position; 
        Forward =  forward;
        Up      =  up;

        m_WorldUp = Up;
    }

    void FlyCamera::Update(float dt)
    {
        // NOTE(Joey): slowly interpolate to target position each frame given some damping factor.
        // This gives smooth camera movement that fades out the closer we are to our target.
        //Position = math::lerp(Position, m_TargetPosition, dt * Damping);

        // NOTE(Joey): calculate the new view matrix
        View = math::lookAt(Position, Position + Forward, Up);
    }

    void FlyCamera::InputKey(float dt, CAMERA_MOVEMENT direction)
    {
        float speed = MovementSpeed * dt;
        // TODO(Joey): define += and -= operators on vectors and re-factor code below.
        if (direction      == CAMERA_FORWARD)
            Position = Position + Forward*speed;
        else if (direction == CAMERA_BACK)
            Position = Position - Forward*speed;
        else if (direction == CAMERA_LEFT)
            Position = Position - Right*speed;
        else if (direction == CAMERA_RIGHT)
            Position = Position + Right*speed;
    }

    void FlyCamera::InputMouse(float dt, float deltaX, float deltaY)
    {
        float xmovement = deltaX * MouseSensitivty * dt;
        float ymovement = deltaY * MouseSensitivty * dt;

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
        math::vec3 newForward;
        newForward.x = cos(0.0174533 * Yaw) * cos(0.0174533 * Pitch);
        newForward.y = sin(0.0174533 * Pitch);
        newForward.z = sin(0.0174533 * Yaw) * cos(0.0174533 * Pitch);
        Forward = math::normalize(newForward);
        Right   = math::normalize(math::cross(Forward, m_WorldUp));
        Up      = math::cross(Right, Forward);
    }
}