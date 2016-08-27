#ifndef CELL_CAMERA_CAMERA_H
#define CELL_CAMERA_CAMERA_H

#include <math/math.h>

#include "camera_frustum.h"

namespace Cell
{
    /* NOTE(Joey):

      Basic root camera. Only does relevant camera calculations 
      with manual forced direction setters. This camera should
      only be used in code and not respond to user input; the
      derived cameras are for user/player interaction.

    */
    // TODO(Joey): do we want the camera to be a SceneNode as well so we can attach it to other entities?
    // what about FPS cameras then? 
    class Camera
    {
    public:
        math::mat4 Projection;
        math::mat4 View;

        math::vec3 Position;
        math::vec3 Forward;
        math::vec3 Up;

        CameraFrustum Frustum;
    private:
        float m_Fov;
        float m_Aspect;
        bool  m_Perspective;
    public:
        // NOTE(Joey): per-frame camera calculations (e.g. damping); specific per camera type
        virtual void Update(float dt) = 0;

        // NOTE(Joey): accept input from windowing library and respond appropriately based on camera type
        virtual void InputKey(unsigned int keyCode, bool firstPress) = 0;
        virtual void InputMouse(float x, float y, float deltaX, float deltaY) = 0;
        
        void SetPerspective(float fov, float aspect, float near, float far);
        void SetOrthographic(float left, float right, float top, float bottom, float near, float far);

        float FrustumHeightAtDistance(float distance);
        float DistanceAtFrustumHeight(float frustumHeight);

    };
}
#endif