#ifndef CELL_CAMERA_FPS_CAMERA_H
#define CELL_CAMERA_FPS_CAMERA_H

#include "camera.h"

namespace Cell
{
    class FpsCamera : public Camera
    {
    public:
        virtual void Update(float dt);

        virtual void InputKey(unsigned int keyCode, bool firstPress);
        virtual void InputMouse(float x, float y, float deltaX, float deltaY);
    };
}
#endif