#ifndef CELL_CAMERA_CAMERA_H
#define CELL_CAMERA_CAMERA_H

#include <math/math.h>

#include "camera_frustum.h"

namespace Cell
{
    // defines several possible options for camera movement. Used as abstraction to stay away from 
    // window-system specific input methods.
    enum CAMERA_MOVEMENT {
        CAMERA_FORWARD,
        CAMERA_BACK,
        CAMERA_LEFT,
        CAMERA_RIGHT,
        CAMERA_UP,
        CAMERA_DOWN,
    };

    /* 

      Basic root camera. Only does relevant camera calculations with manual forced direction 
      setters. This camera should only be used in code and not respond to user input; the
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

        float FOV;
        float Aspect;
        float Near;
        float Far;
        bool  Perspective;

        CameraFrustum Frustum;
    private:
    public:
        Camera(); 
        Camera(math::vec3 position, math::vec3 forward, math::vec3 up);

        void Update(float dt);
        
        void SetPerspective(float fov, float aspect, float near, float far);
        void SetOrthographic(float left, float right, float top, float bottom, float near, float far);

        void UpdateView();

        float FrustumHeightAtDistance(float distance);
        float DistanceAtFrustumHeight(float frustumHeight);

    };
}
#endif