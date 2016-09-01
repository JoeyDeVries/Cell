#include "renderer.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"
#include "../scene/scene_node.h"
#include "../camera/camera.h"

#include "../resources/resources.h"

namespace Cell
{
    Renderer::Renderer()
    {

    }

    void Renderer::Init()
    {
        /* NOTE(Joey):

          Before doing anything with OpenGL we have to load all its functions
          into stored function pointers from the OpenGL driver. Windows by 
          default only hosts a small set of OpenGL functions in its library
          so all the more 'modern' functions have to be manually loaded/
          requested from the graphics cards' OpenGL driver itself.

          This is a cumbersome work as we generally have to store all the
          required function prototypes, function pointer storage and all
          the enum / unique ids of the OpenGL functionality and on top of
          that we have to query for the function's memory location for 
          each and every one of these functions. 

          For this reason we use a library called ... (glad?)

        */
        // [...] 
        

        /* NOTE(Joey): 

          Next up is initializing debug output. Debug output allows us to
          give a callback error reporting function to OpenGL that OpenGL
          will call each time an internal error occurs with detailed 
          information about the error. This helps us to easily catch
          errors from within OpenGL itself saving us from long hours
          of debugging. For more info see:
          http://learnopengl.com/#!In-Practice/Debugging

          Note that debug output will only be initialized if the 
          windowing library initialized an OpenGL context with the
          debug output bit flag set; otherwise this functionality
          is simply ignored.

        */
        // [...]
    }

    void Renderer::SetTarget()
    {

    }

    Camera* Renderer::GetCamera()
    {
        return m_Camera;
    }

    void Renderer::SetCamera(Camera *camera)
    {
        m_Camera = camera;
    }

    void Renderer::PushRender(Mesh &mesh, Material &material)
    {

    }

    void Renderer::PushRender(SceneNode *node)
    {

    }

    void Renderer::Render()
    {

    }
}