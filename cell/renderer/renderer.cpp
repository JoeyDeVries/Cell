#include "renderer.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"
#include "../scene/scene.h"
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

    void Renderer::PushRender(Mesh *mesh, Material *material, math::mat4 transform)
    {
        // NOTE(Joey): don't render right away but push to the command buffer
        // for later rendering.
        m_CommandBuffer.Push(mesh, material, transform);
    }

    void Renderer::PushRender(SceneNode *node)
    {
        // NOTE(Joey): traverse through all the scene nodes and for each node:
        // push its render state to the command buffer together with a 
        // calculated transform matrix.

    }

    void Renderer::PushRender(Scene *scene)
    {
        // NOTE(Joey): propagate scene to SceneNode push call from its top
        // root node, effectively pushing the entire scene.
        PushRender(scene->GetRootNode());
    }

    void Renderer::Render()
    {
        // NOTE(Joey): begin with a forward renderer context for rendering of
        // the command buffer, later switch to a deferred render pipeline once
        // all sub-systems in place work properly.
        
        // NOTE(Joey): sort all render commands and retrieve the sorted array
        m_CommandBuffer.Sort();
        std::vector<RenderCommand> renderCommands = m_CommandBuffer.GetRenderCommands();

        // NOTE(Joey): iterate over all the render commands and execute
        for (unsigned int i = 0; i < renderCommands.size(); ++i)
        {
            Material *material = renderCommands[i].Material;
            Mesh *mesh = renderCommands[i].Mesh;
            
            // TODO(Joey): only use shader and set per-shader specific uniforms
            // (ViewProjection) if state requires change; otherwise ignore.
            material->Shader->Use();
            material->Shader->SetMatrix("projection", m_Camera->Projection);
            material->Shader->SetMatrix("view", m_Camera->View);

            material->Shader->SetMatrix("model", renderCommands[i].Transform);

            if (renderCommands[i].Mesh->Indices.size() > 0)
            {
                // TODO(Joey): parse the proper OpenGL type from the mesh->Toplogy property
                glDrawElements(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_BYTE, 0);
            }
            else
            {
                glDrawArrays(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
            }
        }

        // NOTE(Joey): clear the command buffer s.t. the next frame/call can
        // start from an empty slate again.
        m_CommandBuffer.Clear();

    }
}