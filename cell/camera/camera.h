#ifndef CELL_CAMERA_CAMERA_H
#define CELL_CAMERA_CAMERA_H

#include <math/math.h>

#include "camera_frustum.h"

namespace Cell
{
    // NOTE(Joey): defines several possible options for camera movement. Used as abstraction 
    // to stay away from window-system specific input methods.
    enum CAMERA_MOVEMENT {
        CAMERA_FORWARD,
        CAMERA_BACK,
        CAMERA_LEFT,
        CAMERA_RIGHT,
    };

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

        math::vec3 Position = math::vec3(0.0f, 0.0f,  0.0f);
        math::vec3 Forward  = math::vec3(0.0f, 0.0f, -1.0f);
        math::vec3 Up       = math::vec3(0.0f, 1.0f,  0.0f);
        math::vec3 Right    = math::vec3(1.0f, 0.0f,  0.0f);

        CameraFrustum Frustum;
    private:
        float m_Fov;
        float m_Aspect;
        bool  m_Perspective;
    public:
        Camera() { } // NOTE(Joey): y do I need to make constructor here; See CameraFrustum note.

        // NOTE(Joey): per-frame camera calculations (e.g. damping); specific per camera type
        virtual void Update(float dt) = 0;

        // NOTE(Joey): accept input from windowing library and respond appropriately based on camera type
        virtual void InputKey(float dt, CAMERA_MOVEMENT direction) = 0;
        virtual void InputMouse(float dt, float deltaX, float deltaY) = 0;
        
        void SetPerspective(float fov, float aspect, float near, float far);
        void SetOrthographic(float left, float right, float top, float bottom, float near, float far);

        float FrustumHeightAtDistance(float distance);
        float DistanceAtFrustumHeight(float frustumHeight);

    };
}
#endif