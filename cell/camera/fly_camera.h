#ifndef CELL_CAMERA_FPS_CAMERA_H
#define CELL_CAMERA_FPS_CAMERA_H

#include "camera.h"

#include <math/linear_algebra/vector.h>

namespace Cell
{
	/*
	
	  Derivation of the base camera with support for fly-through motions. Think of WASD forward/right
      type of movement, combined with strafing and free yaw/pitch camera rotation.
	  
	*/
    class FlyCamera : public Camera
    {
	public:
		float Yaw;
		float Pitch;
	
		float MovementSpeed   = 10.0f;
		float MouseSensitivty =  0.1f;
		float Damping         =  5.0f;
	private:
		math::vec3 m_TargetPosition;
		math::vec3 m_WorldUp;
        float m_TargetYaw;
        float m_TargetPitch;
	
    public:
        FlyCamera(math::vec3 position, math::vec3 forward = math::vec3(0.0f, 0.0f, -1.0f), math::vec3 up = math::vec3(0.0f, 1.0f, 0.0f));

        virtual void Update(float dt);

        virtual void InputKey(float dt, CAMERA_MOVEMENT direction);
        virtual void InputMouse(float deltaX, float deltaY);
        virtual void InputScroll(float deltaX, float deltaY);
    };
}
#endif