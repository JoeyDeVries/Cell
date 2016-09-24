#include "renderer.h"

#include "../mesh/mesh.h"
#include "../shading/material.h"
#include "../scene/scene.h"
#include "../scene/scene_node.h"
#include "../camera/camera.h"

#include "../resources/resources.h"

#include <utility/logging/log.h>

#include <stack>

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
        m_CommandBuffer.Push(node->Mesh, node->Material, node->GetTransform());

        // NOTE(Joey): originally a recursive function but transformed to 
        // iterative version by maintaining a stack.
        std::stack<SceneNode*> childStack;
        for (unsigned int i = 0; i < node->GetChildCount(); ++i)
            childStack.push(node->GetChild(i));
        while (!childStack.empty())
        {
            SceneNode *child = childStack.top();
            childStack.pop();
            m_CommandBuffer.Push(child->Mesh, child->Material, child->GetTransform());
            for(unsigned int i = 0; i < child->GetChildCount(); ++i)
                childStack.push(child->GetChild(i));
        }
    }

    void Renderer::PushRender(Scene *scene)
    {
        // NOTE(Joey): propagate scene to SceneNode push call from its top
        // root node, effectively pushing the entire scene.
        PushRender(scene->GetRootNode());
    }

    void Renderer::PushLight(DirectionalLight *light)
    {
        m_DirectionalLights.push_back(light);
    }

    void Renderer::PushLight(PointLight       *light)
    {
        m_PointLights.push_back(light);
    }

    void Renderer::Render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
            Mesh     *mesh     = renderCommands[i].Mesh;

            // NOTE(Joey): set global GL state based on material
            // TODO(Joey): only change these if different value, and sort by major state changes
            glDepthFunc(material->DepthCompare);
            
            // TODO(Joey): only use shader and set per-shader specific uniforms
            // (ViewProjection) if state requires change; otherwise ignore.
            // TODO(Joey): replace w/ uniform buffer objects for 'known' 
            // uniforms that are shared among material runs (like projection/
            // view); do a similar setup for lighting.
            // NOTE(Joey): set default uniforms that are always configured 
            // regardless of shader configuration (see them as a default set of
            // shader uniform variables always there).
            material->Shader->Use();
            material->Shader->SetMatrix("projection", m_Camera->Projection);
            material->Shader->SetMatrix("view",       m_Camera->View);
            material->Shader->SetMatrix("model",      renderCommands[i].Transform);
            // NOTE(Joey): lighting setup: also move to uniform buffer object 
            // in future
            for (unsigned int i = 0; i < m_DirectionalLights.size() && i < 4; ++i) // NOTE(Joey): no more than 4 directional lights
            {
                std::string uniformName = "DirLight" + std::to_string(i) + "_Dir";
                if (material->Shader->HasUniform(uniformName))
                {
                    material->Shader->SetVector(uniformName, m_DirectionalLights[i]->Direction);
                    material->Shader->SetVector("DirLight" + std::to_string(i) + "_Col", m_DirectionalLights[i]->Color);
                }
                else
                    break; // NOTE(Joey): if DirLight2 doesn't exist we assume that DirLight3 and 4,5,6 should also exist; stop searching
            }
            for (unsigned int i = 0; i < m_PointLights.size() && i < 8; ++i) // NOTE(Joey): constrained to max 8 point lights for now in forward context
            {
                std::string uniformName = "PointLight" + std::to_string(i) + "_Pos";
                if (material->Shader->HasUniform(uniformName))
                {
                    material->Shader->SetVector(uniformName, m_PointLights[i]->Position);
                    material->Shader->SetVector("PointLight" + std::to_string(i) + "_Col", m_PointLights[i]->Color);
                }
                else
                    break; // NOTE(Joey): if PointLight2 doesn't exist we assume that PointLight3 and 4,5,6 also don't exist; stop searching
            }

            // NOTE(Joey): bind/active uniform sampler/texture objects
            auto *samplers = material->GetSamplerUniforms();
            for (auto it = samplers->begin(); it != samplers->end(); ++it)
            {
                if(it->second.Type == SHADER_TYPE_SAMPLERCUBE)
                    it->second.TextureCube->Bind(it->second.Unit);
                else
                    it->second.Texture->Bind(it->second.Unit);
            }

            // NOTE(Joey): set uniform state of material
            auto *uniforms = material->GetUniforms();
            for (auto it = uniforms->begin(); it != uniforms->end(); ++it)
            {
                switch (it->second.Type)
                {
                case SHADER_TYPE_BOOL:
                    material->Shader->SetBool(it->first, it->second.Bool);
                    break;
                case SHADER_TYPE_INT:
                    material->Shader->SetInt(it->first, it->second.Int);
                    break;
                case SHADER_TYPE_FLOAT:
                    material->Shader->SetFloat(it->first, it->second.Float);
                    break;
                case SHADER_TYPE_VEC2:
                    material->Shader->SetVector(it->first, it->second.Vec2);
                    break;
                case SHADER_TYPE_VEC3:
                    material->Shader->SetVector(it->first, it->second.Vec3);
                    break;
                case SHADER_TYPE_VEC4:
                    material->Shader->SetVector(it->first, it->second.Vec4);
                    break;
                case SHADER_TYPE_MAT2:
                    material->Shader->SetMatrix(it->first, it->second.Mat2);
                    break;
                case SHADER_TYPE_MAT3:
                    material->Shader->SetMatrix(it->first, it->second.Mat3);
                    break;
                case SHADER_TYPE_MAT4:
                    material->Shader->SetMatrix(it->first, it->second.Mat4);
                    break;
                default:
                    Log::Message("Unrecognized Uniform type set.", LOG_ERROR);
                    break;
                }
            }

            // NOTE(Joey): bind OpenGL render state
            glBindVertexArray(mesh->m_VAO);
            if (mesh->Indices.size() > 0)
            {
                // TODO(Joey): parse the proper OpenGL type from the mesh->Toplogy property
                glDrawElements(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
            }
            else
            {
                glDrawArrays(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
            }
            glBindVertexArray(0); // NOTE(Joey): consider skipping this call without damaging the render pipeline
        }

        // NOTE(Joey): clear the command buffer s.t. the next frame/call can
        // start from an empty slate again.
        m_CommandBuffer.Clear();

        // NOTE(Joey): clear render state
        m_DirectionalLights.clear();
        m_PointLights.clear();

    }
}