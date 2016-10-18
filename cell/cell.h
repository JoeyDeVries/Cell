#ifndef CELL_H
#define CELL_H

#include "renderer/renderer.h"
#include "renderer/render_target.h"
#include "renderer/pbr_environment.h"
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




namespace Cell
{
    /* NOTE(Joey):

      Global entry-point for Cell's initialization. Starts all Cell's relevant
      initialization calls, and makes sure Cell is ready to operate after 
      calling its initialization function. Note that all GL function pointers
      are initialized here as well (including any extensions) w/ GLAD.

    */
    Renderer* Init(GLADloadproc loadProcFunc);

    /* NOTE(Joey): 

      Cleans up Cell of all resources that are no longer relevant.

    */
    void Clean();
}

#endif