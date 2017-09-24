#include "renderer.h"

#include "render_target.h"
#include "MaterialLibrary.h"
#include "PBR.h"
#include "PostProcessor.h"

#include "../mesh/mesh.h"
#include "../mesh/cube.h"
#include "../shading/material.h"
#include "../scene/scene.h"
#include "../scene/scene_node.h"
#include "../camera/camera.h"
#include "../camera/fly_camera.h"
#include "../resources/resources.h"

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/matrix.h>

#include <utility/logging/log.h>
#include <utility/string_id.h>

#include <stack>

namespace Cell
{
    // ------------------------------------------------------------------------
    Renderer::Renderer()
    {

    }
    // ------------------------------------------------------------------------
    Renderer::~Renderer()
    {       
        delete m_NDCPlane;

        delete m_MaterialLibrary;

        delete m_GBuffer;
        delete m_CustomTarget;

        // shadows
        for (int i = 0; i < m_ShadowRenderTargets.size(); ++i)
        {
            delete m_ShadowRenderTargets[i];
        }

        // lighting
        delete m_DebugLightMesh;

        // post-processing
        delete m_PostProcessTarget1;
        delete m_PostProcessor;

        // pbr
        delete m_PBR;      
    }
    // ------------------------------------------------------------------------
    void Renderer::Init(GLADloadproc loadProcFunc)
    {
        // NOTE(Joey): initialize render items
        // TODO(Joey): do we want to abstract this or not? as it is very specific.
        m_NDCPlane = new Quad;
        glGenFramebuffers(1, &m_FramebufferCubemap);
        glGenRenderbuffers(1, &m_CubemapDepthRBO);

        m_CustomTarget = new RenderTarget(1, 1, GL_HALF_FLOAT, 1, true);
        m_PostProcessTarget1 = new RenderTarget(1, 1, GL_UNSIGNED_BYTE, 1, false);
        m_PostProcessor = new PostProcessor(this);

        // lights
        m_DebugLightMesh = new Sphere(16, 16);
        m_DeferredPointMesh = new Sphere(16, 16);

        // deferred renderer
        m_GBuffer = new RenderTarget(1, 1, GL_HALF_FLOAT, 3, true);

        // materials
        m_MaterialLibrary = new MaterialLibrary(m_GBuffer);

        // shadows
        // TODO: think of a more flexible system to allow any number of shadow casters
        // TODO:  (add shadow cast caching; detect light/dynamic-object movement!)
        for (int i = 0; i < 4; ++i) // allow up to a total of 4 dir/spot shadow casters
        {
            RenderTarget *rt = new RenderTarget(2048, 2048, GL_UNSIGNED_BYTE, 1, true);
            m_ShadowRenderTargets.push_back(rt);
        }
       
        // pbr
        m_PBR = new PBR(this);

        // default PBR pre-compute (get a more default oriented HDR map for this)
        Cell::Texture *hdrMap = Cell::Resources::LoadHDR("hdr factory catwalk", "textures/backgrounds/alley.hdr");
        Cell::PBRCapture *envBridge = m_PBR->ProcessEquirectangular(hdrMap);
        SetSkyCapture(envBridge);
    }
    // ------------------------------------------------------------------------
    void Renderer::SetRenderSize(unsigned int width, unsigned int height)
    {
        m_RenderSize.x = width;
        m_RenderSize.y = height;

        m_CustomTarget->Resize(width, height);
        m_PostProcessTarget1->Resize(width, height);
        
        m_PostProcessor->UpdateRenderSize(width, height);
    }
    // ------------------------------------------------------------------------
    math::vec2 Renderer::GetRenderSize()
    {
        return m_RenderSize;
    }
    // ------------------------------------------------------------------------
    void Renderer::SetTarget(RenderTarget *renderTarget, GLenum target)
    {
        m_CurrentRenderTargetCustom = renderTarget;
        if (renderTarget != nullptr) 
        {
            if (std::find(m_RenderTargetsCustom.begin(), m_RenderTargetsCustom.end(), renderTarget)
                == m_RenderTargetsCustom.end())
            {
                m_RenderTargetsCustom.push_back(renderTarget);
            }
        }
    }
    // ------------------------------------------------------------------------
    Camera* Renderer::GetCamera()
    {
        return m_Camera;
    }
    // ------------------------------------------------------------------------
    void Renderer::SetCamera(Camera *camera)
    {
        m_Camera = camera;
    }
    // ------------------------------------------------------------------------
    Material* Renderer::CreateMaterial(std::string base)
    {
        return m_MaterialLibrary->CreateMaterial(base);      
    }
    // ------------------------------------------------------------------------
    Material* Renderer::CreateCustomMaterial(Shader *shader)
    {
        return m_MaterialLibrary->CreateCustomMaterial(shader);
    }
    // ------------------------------------------------------------------------
    Material* Renderer::CreatePostProcessingMaterial(Shader *shader)
    {
        return m_MaterialLibrary->CreatePostProcessingMaterial(shader);
    }
    // ------------------------------------------------------------------------
    void Renderer::PushRender(Mesh *mesh, Material *material, math::mat4 transform)
    {
        // get current render target
        RenderTarget *target = getCurrentRenderTarget();
        // don't render right away but push to the command buffer for later rendering.
        m_CommandBuffer.Push(mesh, material, transform, target);
    }
    // ------------------------------------------------------------------------
    void Renderer::PushRender(SceneNode *node)
    {
        // get current render target
        RenderTarget *target = getCurrentRenderTarget();
        // NOTE(Joey): traverse through all the scene nodes and for each node:
        // push its render state to the command buffer together with a 
        // calculated transform matrix.
        // But only push the first command if it has a mesh/material, otherwise it's likely a 
        // container scene node.
        if (node->Mesh) 
        {
            m_CommandBuffer.Push(node->Mesh, node->Material, node->GetTransform(), target);
        }
        else
        {
            // do make sure we do always update the parent's transform.
            // TODO: this needs to go!
            node->GetTransform();
        }

        // NOTE(Joey): originally a recursive function but transformed to 
        // iterative version by maintaining a stack.
        std::stack<SceneNode*> childStack;
        for (unsigned int i = 0; i < node->GetChildCount(); ++i)
            childStack.push(node->GetChildByIndex(i));
        while (!childStack.empty())
        {
            SceneNode *child = childStack.top();
            childStack.pop();
            // NOTE(Joey): again, only push render command if the child isn't a container node.
            if (child->Mesh)
            {
                m_CommandBuffer.Push(child->Mesh, child->Material, child->GetTransform(), target);
            }
            else
            {
                // do make sure we do always update the parent's transform.
                // TODO: this needs to go!
                node->GetTransform();
            }
            for(unsigned int i = 0; i < child->GetChildCount(); ++i)
                childStack.push(child->GetChildByIndex(i));
        }
    }
    // ------------------------------------------------------------------------
    void Renderer::PushPostProcessor(Material *postProcessor)
    {
        // we only care about the material, mesh as NDC quad is pre-defined.
        m_CommandBuffer.Push(nullptr, postProcessor, math::mat4());
    }
    // ------------------------------------------------------------------------
    void Renderer::AddLight(DirectionalLight *light)
    {
        m_DirectionalLights.push_back(light);
    }
    // ------------------------------------------------------------------------
    void Renderer::AddLight(PointLight *light)
    {
        m_PointLights.push_back(light);
    }
    // ------------------------------------------------------------------------
    // TODO(Joey): pass camera to RenderPushedCommands? Think it makes sense.
    void Renderer::RenderPushedCommands()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* NOTE(Joey): 
        
          - First we do the default light/render passes as the deferred pass here.
          - Then we do the forward 'custom' rendering pass where developers can
            write their own shaders and render stuff as they want, not sacrifcing
            flexibility; this also includes custom render targets.
          - Then the alpha blended rendering pass.
          - Then we do post-processing, one can supply their own post-processing
            materials by setting the 'post-processing' flag of the material.
            Each material flagged as post-processing will run after the default
            post-processing shaders (before/after HDR-tonemap/gamma-correct?).

        */
        m_CommandBuffer.Sort();

        // NOTE(Joey); deferred here:
        std::vector<RenderCommand>& deferredRenderCommands = m_CommandBuffer.GetDeferredRenderCommands();
        // 1. Geometry buffer
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer->ID);
        unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);
        if (m_GBuffer->Width != m_RenderSize.x || m_GBuffer->Height != m_RenderSize.y)
        {
            m_GBuffer->Resize(m_RenderSize.x, m_RenderSize.y);
        }
        m_Camera->SetPerspective(m_Camera->FOV, m_RenderSize.x / m_RenderSize.y, 0.1, 100.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        for (unsigned int i = 0; i < deferredRenderCommands.size(); ++i)
        {
            renderDeferredCommand(&deferredRenderCommands[i], m_Camera);
        }

        //attachments[0] = GL_NONE; // disable for next pass (shadow map generation)
        attachments[1] = GL_NONE;
        attachments[2] = GL_NONE;
        glDrawBuffers(3, attachments);

        // 1.5. render all shadow casters to light shadow buffers
        glCullFace(GL_FRONT);
        std::vector<RenderCommand> shadowRenderCommands = m_CommandBuffer.GetShadowCastRenderCommands();
        m_ShadowViewProjections.clear();

        unsigned int shadowRtIndex = 0;
        for (int i = 0; i < m_DirectionalLights.size(); ++i)
        {
            DirectionalLight* light = m_DirectionalLights[i];
            if (light->CastShadows)
            {
                m_MaterialLibrary->dirShadowShader->Use();

                glBindFramebuffer(GL_FRAMEBUFFER, m_ShadowRenderTargets[shadowRtIndex]->ID);
                glViewport(0, 0, m_ShadowRenderTargets[shadowRtIndex]->Width, m_ShadowRenderTargets[shadowRtIndex]->Height);
                glClear(GL_DEPTH_BUFFER_BIT);

                math::mat4 lightProjection = math::orthographic(-20.0f, 20.0f, 20.0f, -20.0f, -15.0f, 20.0f);
                math::mat4 lightView = math::lookAt(-light->Direction * 10.0f, math::vec3(0.0), math::vec3::UP);
                m_DirectionalLights[i]->LightSpaceViewProjection = lightProjection * lightView;
                m_DirectionalLights[i]->ShadowMapRT = m_ShadowRenderTargets[shadowRtIndex];
                for (int j = 0; j < shadowRenderCommands.size(); ++j)
                {
                    renderShadowCastCommand(&shadowRenderCommands[j], lightProjection, lightView);
                }
                ++shadowRtIndex;
            }
        }
        glCullFace(GL_BACK);
        attachments[0] = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(3, attachments);

        // 2. do post-processing steps before lighting stage (e.g. SSAO)
        m_PostProcessor->ProcessPreLighting(this, m_GBuffer, m_Camera);


        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
        glViewport(0, 0, m_CustomTarget->Width, m_CustomTarget->Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 3. Render deferred shader for each light (full quad for directional, spheres for point lights)
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        // bind gbuffer
        m_GBuffer->GetColorTexture(0)->Bind(0);
        m_GBuffer->GetColorTexture(1)->Bind(1);
        m_GBuffer->GetColorTexture(2)->Bind(2);
        
        // ambient lighting
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        renderDeferredAmbient();
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        // directional lights
        for (auto it = m_DirectionalLights.begin(); it != m_DirectionalLights.end(); ++it)
        {
            renderDeferredDirLight(*it);
        }
        // point lights
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
        {
            //renderDeferredPointLight(*it);
        }
        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_BLEND);

        // 4. blit depth buffer to default for forward rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_CustomTarget->ID); // write to default framebuffer
        glBlitFramebuffer(
            0, 0, m_GBuffer->Width, m_GBuffer->Height, 0, 0, m_RenderSize.x, m_RenderSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        // 5. custom forward render pass 

        // NOTE(Joey): push default render target to the end of the render
        // target buffer s.t. we always render the default buffer last.
        // TODO(Joey): check if this is a practical approach, to make this
        // assumption and otherwise configure order of targets within
        // render commands.
        m_RenderTargetsCustom.push_back(nullptr);

        // TODO(Joey): note that if no render target is specified, the default 
        // nullptr renderTarget SHOULD be in there; make sure to check this is
        // always the case! If it isn't, there should be 0 render commands 
        // pushed.
        for (unsigned int targetIndex = 0; targetIndex < m_RenderTargetsCustom.size(); ++targetIndex)
        {
            RenderTarget *renderTarget = m_RenderTargetsCustom[targetIndex];
            if (renderTarget)
            {
                glViewport(0, 0, renderTarget->Width, renderTarget->Height);
                glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->ID);
                if (renderTarget->HasDepthAndStencil)
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                else
                    glClear(GL_COLOR_BUFFER_BIT);
                // TODO(Joey): enable multiple color targets
                // TODO(Joey): more configurable camera settings, what about orthographic? and near/far plane?
                m_Camera->SetPerspective(m_Camera->FOV, 
                                         (float)renderTarget->Width / (float)renderTarget->Height, 
                                         0.1, 100.0f); 
            }
            else
            {
                // NOTE(Joey): don't render to default framebuffer, but to custom target framebuffer
                // which we'll use for post-processing.
                glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
                glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
                m_Camera->SetPerspective(m_Camera->FOV, m_RenderSize.x / m_RenderSize.y, 0.1, 
                                         100.0f);
            }

            // sort all render commands and retrieve the sorted array
            std::vector<RenderCommand>& renderCommands = m_CommandBuffer.GetCustomRenderCommands(renderTarget);

            // terate over all the render commands and execute
            for (unsigned int i = 0; i < renderCommands.size(); ++i)
            {
                renderCustomCommand(&renderCommands[i], m_Camera);
            }
        }

        // 6. post-processing stage after all lighting calculations 
        m_PostProcessor->ProcessPostLighting(this, m_GBuffer, m_CustomTarget, m_Camera);

        // 7. render (debug) visuals
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
        for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
        {
            if ((*it)->RenderMesh)
            {
                m_MaterialLibrary->debugLightMaterial->SetVector("lightColor", (*it)->Color);

                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glDisable(GL_CULL_FACE);
                RenderCommand command;
                command.Material = m_MaterialLibrary->debugLightMaterial;
                command.Mesh = m_DebugLightMesh;
                math::mat4 model;
                math::translate(model, (*it)->Position);
                math::scale(model, math::vec3((*it)->Radius));
                command.Transform = model;

                renderCustomCommand(&command, m_Camera);
                glEnable(GL_CULL_FACE);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        //m_PBR->RenderProbes();

        // 8. custom post-processing pass
        std::vector<RenderCommand>& postProcessingCommands = m_CommandBuffer.GetPostProcessingRenderCommands();
        for (unsigned int i = 0; i < postProcessingCommands.size(); ++i)
        {
            // NOTE(Joey): ping-pong between render textures
            bool even = i % 2 == 0;
            Blit(even ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0),
                 even ? m_PostProcessTarget1 : m_CustomTarget, 
                 postProcessingCommands[i].Material);
        }

        // 9. final post-processing steps, blitting to default framebuffer
        m_PostProcessor->Blit(this, postProcessingCommands.size() % 2 == 0 ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0));

        //Blit(m_PostProcessor->BlurredEightOutput, nullptr);
        //Blit(m_ShadowRenderTargets[0]->GetColorTexture(0), nullptr);



        // clear the command buffer s.t. the next frame/call can start from an empty slate again.
        m_CommandBuffer.Clear();

        // clear render state
        m_RenderTargetsCustom.clear();
        // TODO(Joey): this can probably be done in a cleaner fashion:
        //m_RenderTargetsCustom.push_back(nullptr); // NOTE(Joey): always add the default render 
        // target to the queue.
        m_CurrentRenderTargetCustom = nullptr;

    }
    // ------------------------------------------------------------------------
    void Renderer::Blit(Texture *src,
                        RenderTarget *dst, 
                        Material     *material, 
                        std::string   textureUniformName)
    {
        // if a destination target is given, bind to its framebuffer
        if (dst)
        {
            glViewport(0, 0, dst->Width, dst->Height);
            glBindFramebuffer(GL_FRAMEBUFFER, dst->ID);
            if (dst->HasDepthAndStencil)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            else
                glClear(GL_COLOR_BUFFER_BIT);
        }
        // else we bind to the default framebuffer
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        // if no material is given, use default blit material
        if (!material)
        {
            material = m_MaterialLibrary->defaultBlitMaterial;
        }
        // if a source render target is given, use its color buffer as input to material shader.
        if (src)
        {
            material->SetTexture(textureUniformName, src, 0);
        }
        // render screen-space material to quad which will be displayed in dst's buffers.
        RenderCommand command;
        command.Material = material;
        command.Mesh = m_NDCPlane;
        renderCustomCommand(&command, nullptr);
    }   
    // ------------------------------------------------------------------------
    void Renderer::SetSkyCapture(PBRCapture* pbrEnvironment)
    {
        m_PBR->SetSkyCapture(pbrEnvironment);
    }
    // ------------------------------------------------------------------------
    PBRCapture* Renderer::GetSkypCature()
    {
        return m_PBR->GetSkyCapture();
    }
    // ------------------------------------------------------------------------
    void Renderer::AddIrradianceProbe(math::vec3 position, float radius)
    {
        m_ProbeSpatials.push_back(math::vec4(position, radius));
    }
    // ------------------------------------------------------------------------
    void Renderer::BakeProbes(SceneNode* scene)
    {
        if(!scene)
        {
            // if no scene node was provided, use root node (capture all)
            scene = Scene::Root;
        }
        // build a command list of nodes within the reflection probe's capture box/radius.
        CommandBuffer commandBuffer;
        std::vector<Material*> materials;
     
        // originally a recursive function but transformed to iterative version
        std::stack<SceneNode*> sceneStack;
        sceneStack.push(scene);
        while (!sceneStack.empty())
        {
            SceneNode *node = sceneStack.top();
            sceneStack.pop();
            if (node->Mesh)
            {
                auto samplerUniforms = *(node->Material->GetSamplerUniforms());
                if (samplerUniforms.find("TexAlbedo") != samplerUniforms.end())
                {
                    materials.push_back(new Material(m_PBR->m_ProbeCaptureShader));
                    materials[materials.size() - 1]->SetTexture("TexAlbedo", samplerUniforms["TexAlbedo"].Texture, 0);
                    if (samplerUniforms.find("TexNormal") != samplerUniforms.end())
                    {
                        materials[materials.size() - 1]->SetTexture("TexNormal", samplerUniforms["TexNormal"].Texture, 1);
                    }
                    if (samplerUniforms.find("TexMetallic") != samplerUniforms.end())
                    {
                        materials[materials.size() - 1]->SetTexture("TexMetallic", samplerUniforms["TexMetallic"].Texture, 2);
                    }
                    if (samplerUniforms.find("TexRoughness") != samplerUniforms.end())
                    {
                        materials[materials.size() - 1]->SetTexture("TexRoughness", samplerUniforms["TexRoughness"].Texture, 3);
                    }
                    commandBuffer.Push(node->Mesh, materials[materials.size() - 1], node->GetTransform());
                }
                else if (samplerUniforms.find("background") != samplerUniforms.end())
                {   // we have a background scene node, add those as well
                    commandBuffer.Push(node->Mesh, node->Material, node->GetTransform());
                }
            }
            else
            {
                // do make sure we do always update the parent's transform.
                // TODO: this needs to go!
                node->GetTransform();
            }
            for (unsigned int i = 0; i < node->GetChildCount(); ++i)
                sceneStack.push(node->GetChildByIndex(i));
        }
        commandBuffer.Sort();
        std::vector<RenderCommand> renderCommands = commandBuffer.GetCustomRenderCommands(nullptr);

        m_PBR->ClearIrradianceProbes();
        for (int i = 0; i < m_ProbeSpatials.size(); ++i)
        {
            TextureCube renderResult;
            renderResult.DefaultInitialize(32, 32, GL_RGB, GL_FLOAT);

            renderToCubemap(renderCommands, &renderResult, m_ProbeSpatials[i].xyz);

            PBRCapture* capture = m_PBR->ProcessCube(&renderResult, false);
            m_PBR->AddIrradianceProbe(capture, m_ProbeSpatials[i].xyz, m_ProbeSpatials[i].w);
        }

        for (int i = 0; i < materials.size(); ++i)
        {
            delete materials[i];
        }
    }
    // ------------------------------------------------------------------------
    void Renderer::renderDeferredCommand(RenderCommand *command, Camera *camera)
    {
        Material *material = command->Material;
        Mesh     *mesh     = command->Mesh;        

        // TODO(Joey): only use shader and set per-shader specific uniforms
        // (ViewProjection) if state requires change; otherwise ignore.
        // TODO(Joey): replace w/ uniform buffer objects for 'known' 
        // uniforms that are shared among material runs (like projection/
        // view); do a similar setup for lighting.
        // TODO(Joey): I've decided that the only lighting pass that is default
        // supported will be the deferred pass, if other shaders want lighting
        // information as well they'll have to add it themselves.
        // NOTE(Joey): set default uniforms that are always configured 
        // regardless of shader configuration (see them as a default set of
        // shader uniform variables always there).
        material->GetShader()->Use();
        if (camera)
        {
            material->GetShader()->SetMatrix("projection", camera->Projection);
            material->GetShader()->SetMatrix("view", camera->View);
            material->GetShader()->SetMatrix("model", command->Transform);
            material->GetShader()->SetVector("CamPos", camera->Position);
        }      

        // NOTE(Joey): bind/active uniform sampler/texture objects
        auto *samplers = material->GetSamplerUniforms();
        for (auto it = samplers->begin(); it != samplers->end(); ++it)
        {
            if (it->second.Type == SHADER_TYPE_SAMPLERCUBE)
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
                material->GetShader()->SetBool(it->first, it->second.Bool);
                break;
            case SHADER_TYPE_INT:
                material->GetShader()->SetInt(it->first, it->second.Int);
                break;
            case SHADER_TYPE_FLOAT:
                material->GetShader()->SetFloat(it->first, it->second.Float);
                break;
            case SHADER_TYPE_VEC2:
                material->GetShader()->SetVector(it->first, it->second.Vec2);
                break;
            case SHADER_TYPE_VEC3:
                material->GetShader()->SetVector(it->first, it->second.Vec3);
                break;
            case SHADER_TYPE_VEC4:
                material->GetShader()->SetVector(it->first, it->second.Vec4);
                break;
            case SHADER_TYPE_MAT2:
                material->GetShader()->SetMatrix(it->first, it->second.Mat2);
                break;
            case SHADER_TYPE_MAT3:
                material->GetShader()->SetMatrix(it->first, it->second.Mat3);
                break;
            case SHADER_TYPE_MAT4:
                material->GetShader()->SetMatrix(it->first, it->second.Mat4);
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
            glDrawElements(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
        }
        glBindVertexArray(0); // NOTE(Joey): consider skipping this call without damaging the render pipeline
    }
    // ------------------------------------------------------------------------
    void Renderer::renderCustomCommand(RenderCommand *command, Camera *camera)
    {
        Material *material = command->Material;
        Mesh     *mesh     = command->Mesh;

        // NOTE(Joey): set global GL state based on material
        // TODO(Joey): only change these if different value, and sort by major state changes
        //             write a state cache.
        glDepthFunc(material->DepthCompare);
        if (material->DepthTest)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
        if (material->Blend)
        {
            glEnable(GL_BLEND);
            glBlendFunc(material->BlendSrc, material->BlendDst);
        }
        else
        {
            glDisable(GL_BLEND);
        }
        if (material->Cull)
        {
            glEnable(GL_CULL_FACE);
        }
        else
        {
            glDisable(GL_CULL_FACE);
        }


        // TODO(Joey): only use shader and set per-shader specific uniforms
        // (ViewProjection) if state requires change; otherwise ignore.
        // TODO(Joey): replace w/ uniform buffer objects for 'known' 
        // uniforms that are shared among material runs (like projection/
        // view); do a similar setup for lighting.
        // TODO(Joey): I've decided that the only lighting pass that is default
        // supported will be the deferred pass, if other shaders want lighting
        // information as well they'll have to add it themselves.
        // NOTE(Joey): set default uniforms that are always configured 
        // regardless of shader configuration (see them as a default set of
        // shader uniform variables always there).
        material->GetShader()->Use();
        if (camera)
        {
            material->GetShader()->SetMatrix("projection", camera->Projection);
            material->GetShader()->SetMatrix("view", camera->View);
            material->GetShader()->SetMatrix("model", command->Transform);
            material->GetShader()->SetVector("CamPos", camera->Position);
        }
        // NOTE(Joey): lighting setup: also move to uniform buffer object in future
        for (unsigned int i = 0; i < m_DirectionalLights.size() && i < 4; ++i) // NOTE(Joey): no more than 4 directional lights
        {
            std::string uniformName = "DirLight" + std::to_string(i) + "_Dir";
            if (material->GetShader()->HasUniform(uniformName))
            {
                material->GetShader()->SetVector(uniformName, m_DirectionalLights[i]->Direction);
                material->GetShader()->SetVector("DirLight" + std::to_string(i) + "_Col", m_DirectionalLights[i]->Color);
            }
            else
                break; // NOTE(Joey): if DirLight2 doesn't exist we assume that DirLight3 and 4,5,6 also do not exist; stop searching
        }
        for (unsigned int i = 0; i < m_PointLights.size() && i < 8; ++i) // NOTE(Joey): constrained to max 8 point lights for now in forward context
        {
            std::string uniformName = "PointLight" + std::to_string(i) + "_Pos";
            if (material->GetShader()->HasUniform(uniformName))
            {
                material->GetShader()->SetVector(uniformName, m_PointLights[i]->Position);
                material->GetShader()->SetVector("PointLight" + std::to_string(i) + "_Col", m_PointLights[i]->Color);
            }
            else
                break; // NOTE(Joey): if PointLight2 doesn't exist we assume that PointLight3 and 4,5,6 also don't exist; stop searching
        }

        // NOTE(Joey): bind/active uniform sampler/texture objects
        auto *samplers = material->GetSamplerUniforms();
        for (auto it = samplers->begin(); it != samplers->end(); ++it)
        {
            if (it->second.Type == SHADER_TYPE_SAMPLERCUBE)
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
                material->GetShader()->SetBool(it->first, it->second.Bool);
                break;
            case SHADER_TYPE_INT:
                material->GetShader()->SetInt(it->first, it->second.Int);
                break;
            case SHADER_TYPE_FLOAT:
                material->GetShader()->SetFloat(it->first, it->second.Float);
                break;
            case SHADER_TYPE_VEC2:
                material->GetShader()->SetVector(it->first, it->second.Vec2);
                break;
            case SHADER_TYPE_VEC3:
                material->GetShader()->SetVector(it->first, it->second.Vec3);
                break;
            case SHADER_TYPE_VEC4:
                material->GetShader()->SetVector(it->first, it->second.Vec4);
                break;
            case SHADER_TYPE_MAT2:
                material->GetShader()->SetMatrix(it->first, it->second.Mat2);
                break;
            case SHADER_TYPE_MAT3:
                material->GetShader()->SetMatrix(it->first, it->second.Mat3);
                break;
            case SHADER_TYPE_MAT4:
                material->GetShader()->SetMatrix(it->first, it->second.Mat4);
                break;
            default:
                Log::Message("Unrecognized Uniform type set.", LOG_ERROR);
                break;
            }
        }

        renderMesh(mesh, material->GetShader());
    }
    // ------------------------------------------------------------------------
    void Renderer::renderToCubemap(SceneNode *  scene,
        TextureCube *target,
        math::vec3   position,
        unsigned int mipLevel)
    {
        // NOTE(Joey): create a command buffer specifically for this operation (as to not conflict 
        // with main command buffer)
        CommandBuffer commandBuffer;
        // TODO(Joey): code duplication! re-factor!
        commandBuffer.Push(scene->Mesh, scene->Material, scene->GetTransform());
        // NOTE(Joey): originally a recursive function but transformed to 
        // iterative version by maintaining a stack.
        std::stack<SceneNode*> childStack;
        for (unsigned int i = 0; i < scene->GetChildCount(); ++i)
            childStack.push(scene->GetChildByIndex(i));
        while (!childStack.empty())
        {
            SceneNode *child = childStack.top();
            childStack.pop();
            commandBuffer.Push(child->Mesh, child->Material, child->GetTransform());
            for (unsigned int i = 0; i < child->GetChildCount(); ++i)
                childStack.push(child->GetChildByIndex(i));
        }
        commandBuffer.Sort();
        std::vector<RenderCommand> renderCommands = commandBuffer.GetCustomRenderCommands(nullptr);

        renderToCubemap(renderCommands, target, position, mipLevel);
    }
    // ------------------------------------------------------------------------
    void Renderer::renderToCubemap(std::vector<RenderCommand>& renderCommands, TextureCube* target, math::vec3 position, unsigned int mipLevel)
    {
        // define 6 camera directions/lookup vectors
        Camera faceCameras[6] = {
            Camera(position, math::vec3(1.0f,  0.0f,  0.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3(-1.0f,  0.0f,  0.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3(0.0f,  1.0f,  0.0f), math::vec3(0.0f,  0.0f,  1.0f)),
            Camera(position, math::vec3(0.0f, -1.0f,  0.0f), math::vec3(0.0f,  0.0f,-1.0f)),
            Camera(position, math::vec3(0.0f,  0.0f,  1.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3(0.0f,  0.0f, -1.0f), math::vec3(0.0f, -1.0f,  0.0f))
        };

        // resize target dimensions based on mip level we're rendering.
        float width = (float)target->FaceWidth * pow(0.5, mipLevel);
        float height = (float)target->FaceHeight * pow(0.5, mipLevel);

        // TODO(Joey): only resize/recreate if faceWidth is different than before
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CubemapDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
            m_CubemapDepthRBO);

        // resize relevant buffers
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);

        for (unsigned int i = 0; i < 6; ++i)
        {
            Camera *camera = &faceCameras[i];
            camera->SetPerspective(math::Deg2Rad(90.0f), width / height, 0.1f, 100.0f);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, target->ID, mipLevel);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (unsigned int i = 0; i < renderCommands.size(); ++i)
            {
                // cubemap generation only works w/ custom materials 
                assert(renderCommands[i].Material->Type == MATERIAL_CUSTOM);
                renderCustomCommand(&renderCommands[i], camera);
            }
        }
        // reset state
        // TODO: is this still necessary (global render targets managed at a higher level)?
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderMesh(Mesh* mesh, Shader* shader)
    {
        glBindVertexArray(mesh->m_VAO);
        if (mesh->Indices.size() > 0)
        {
            glDrawElements(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, mesh->Indices.size(), GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(mesh->Topology == TRIANGLE_STRIP ? GL_TRIANGLE_STRIP : GL_TRIANGLES, 0, mesh->Positions.size());
        }
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::updateGlobalUBOs()
    {
        // TODO(Joey): decide on std140 layout for global variables (do we split this on the deferred/forward/post pass?)
    }
    // --------------------------------------------------------------------------------------------
    RenderTarget* Renderer::getCurrentRenderTarget()
    {
        return m_CurrentRenderTargetCustom;
        //if(m_RenderTargetsCustom.size() > 0)
            //return m_RenderTargetsCustom[m_RenderTargetsCustom.size() - 1];
        //else
            //return nullptr;
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredAmbient()
    {
        PBRCapture* skyCapture = m_PBR->GetSkyCapture();        
        skyCapture->Prefiltered->Bind(4);

        m_PBR->m_RenderTargetBRDFLUT->GetColorTexture(0)->Bind(5);
        m_PostProcessor->SSAOOutput->Bind(6);

        //std::vector<PBRCapture*> irradianceProbes = m_PBR->GetIrradianceProbes(math::vec3(0.0f), 25.0f);
        auto irradianceProbes = m_PBR->m_CaptureProbes;
        for (int i = 0; i < irradianceProbes.size(); ++i)
        {
            PBRCapture* probe = irradianceProbes[i];
            probe->Irradiance->Bind(3);

            Shader* irradianceShader = m_MaterialLibrary->deferredIrradianceShader;
            irradianceShader->Use();
            irradianceShader->SetVector("camPos", m_Camera->Position);
            irradianceShader->SetVector("probePos", probe->Position);
            irradianceShader->SetFloat("probeRadius", probe->Radius);

            math::mat4 model;
            math::translate(model, probe->Position);
            math::scale(model, math::vec3(probe->Radius));
            irradianceShader->SetMatrix("projection", m_Camera->Projection);
            irradianceShader->SetMatrix("view", m_Camera->View);
            irradianceShader->SetMatrix("model", model);

            renderMesh(m_DeferredPointMesh, irradianceShader);
        }
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredDirLight(DirectionalLight *light)
    {
        Shader* dirShader = m_MaterialLibrary->deferredDirectionalShader;

        dirShader->Use();
        dirShader->SetVector("camPos", m_Camera->Position);
        dirShader->SetVector("lightDir", light->Direction);
        dirShader->SetVector("lightColor", math::normalize(light->Color) * light->Intensity); // TODO(Joey): enforce light normalization with light setter?
        dirShader->SetMatrix("view", m_Camera->View);

        if (light->ShadowMapRT)
        {
            dirShader->SetMatrix("lightShadowViewProjection", light->LightSpaceViewProjection);
            light->ShadowMapRT->GetDepthStencilTexture()->Bind(3);
        }
            
        renderMesh(m_NDCPlane, dirShader);
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredPointLight(PointLight *light)
    {
        Shader *pointShader = m_MaterialLibrary->deferredPointShader;

        pointShader->Use();
        pointShader->SetVector("camPos", m_Camera->Position);
        pointShader->SetVector("lightPos", light->Position);
        pointShader->SetFloat("lightRadius", light->Radius);
        pointShader->SetVector("lightColor", math::normalize(light->Color) * light->Intensity);

        math::mat4 model;
        math::translate(model, light->Position);
        math::scale(model, math::vec3(light->Radius));
        pointShader->SetMatrix("projection", m_Camera->Projection);
        pointShader->SetMatrix("view", m_Camera->View);
        pointShader->SetMatrix("model", model);

        renderMesh(m_DeferredPointMesh, pointShader);    
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderShadowCastCommand(RenderCommand* command, const math::mat4& projection, const math::mat4& view)
    {
        Shader* shadowShader = m_MaterialLibrary->dirShadowShader;

        shadowShader->SetMatrix("projection", projection);
        shadowShader->SetMatrix("view", view);
        shadowShader->SetMatrix("model", command->Transform);

        renderMesh(command->Mesh, shadowShader);
    }
}