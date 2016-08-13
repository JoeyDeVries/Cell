#include "renderer.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"

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

          For this reason we use a library called ...

        */

    }

    void Renderer::SetTarget()
    {

    }

    void Renderer::PushRender(Mesh &mesh, Material &material)
    {

    }

    void Renderer::Render()
    {

    }
}