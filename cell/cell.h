#ifndef CELL_H
#define CELL_H

#include "renderer/renderer.h"
#include "renderer/render_target.h"
#include "renderer/pbr_capture.h"
#include "camera/fly_camera.h"
#include "resources/resources.h"
#include "mesh/mesh.h"
#include "mesh/quad.h"
#include "mesh/circle.h"
#include "mesh/line_strip.h"
#include "mesh/cube.h"
#include "mesh/plane.h"
#include "mesh/sphere.h"
#include "mesh/torus.h"
#include "shading/shader.h"
#include "shading/material.h"
#include "shading/texture.h"
#include "shading/texture_cube.h"
#include "scene/scene.h"
#include "scene/scene_node.h"
#include "scene/background.h"

#include "glad/glad.h"

class GLFWwindow;

namespace Cell
{
    /* 

      Global entry-point for Cell's initialization. Starts all Cell's relevant initialization 
      calls, and makes sure Cell is ready to operate after calling its initialization function. 
      Note that all GL function pointers are initialized here as well (including any extensions) 
      w/ GLAD.

    */
    Renderer* Init(GLFWwindow* window, GLADloadproc loadProcFunc);


    // cleans up Cell of all resources that are no longer relevant.
    void Clean();

    /* 

      Initializes all render data required for processing a new frame. This is mostly relevant
      for rendering GUI items.

    */
    void NewFrame();

    /* 

      Renders (debugging) GUI (IMGUI) on top of Cell's main renderer, by default the renderer's 
      configuration UI is rendered as well, which can be enabled/disabled on demand.

    */
    void RenderGUI();

    // pass mouse input to Cell
    void InputMouse(int button, int action);

    // pass keyboard input to Cell
    void InputKey(int key, int action);

    // pass scroll input to Cell
    void InputScroll(float scrollOffset);
}

#endif