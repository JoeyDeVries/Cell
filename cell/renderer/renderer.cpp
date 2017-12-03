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
#include "../imgui/imgui.h"

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
        delete m_CommandBuffer;

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
        // initialize render items
        m_CommandBuffer = new CommandBuffer(this);

        // configure default OpenGL state
        m_GLCache.SetDepthTest(true);
        m_GLCache.SetCull(true);
        glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

        glViewport(0.0f, 0.0f, m_RenderSize.x, m_RenderSize.y);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);

        m_NDCPlane = new Quad;
        glGenFramebuffers(1, &m_FramebufferCubemap);
        glGenRenderbuffers(1, &m_CubemapDepthRBO);

        m_CustomTarget       = new RenderTarget(1, 1, GL_HALF_FLOAT, 1, true);
        m_PostProcessTarget1 = new RenderTarget(1, 1, GL_UNSIGNED_BYTE, 1, false);
        m_PostProcessor      = new PostProcessor(this);

        // lights
        m_DebugLightMesh    = new Sphere(16, 16);
        m_DeferredPointMesh = new Sphere(16, 16);

        // deferred renderer
        m_GBuffer = new RenderTarget(1, 1, GL_HALF_FLOAT, 4, true);

        // materials
        m_MaterialLibrary = new MaterialLibrary(m_GBuffer);

        // shadows
        for (int i = 0; i < 4; ++i) // allow up to a total of 4 dir/spot shadow casters
        {
            RenderTarget *rt = new RenderTarget(2048, 2048, GL_UNSIGNED_BYTE, 1, true);
            rt->m_DepthStencil.Bind();
            rt->m_DepthStencil.SetFilterMin(GL_NEAREST);
            rt->m_DepthStencil.SetFilterMax(GL_NEAREST);
            rt->m_DepthStencil.SetWrapMode(GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
            m_ShadowRenderTargets.push_back(rt);
        }
       
        // pbr
        m_PBR = new PBR(this);

        // ubo
        glGenBuffers(1, &m_GlobalUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUBO);
        glBufferData(GL_UNIFORM_BUFFER, 720, nullptr, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_GlobalUBO);

        // default PBR pre-compute (get a more default oriented HDR map for this)
        Cell::Texture *hdrMap = Cell::Resources::LoadHDR("sky env", "textures/backgrounds/alley.hdr");
        Cell::PBRCapture *envBridge = m_PBR->ProcessEquirectangular(hdrMap);
        SetSkyCapture(envBridge);
    }
    // ------------------------------------------------------------------------
    void Renderer::SetRenderSize(unsigned int width, unsigned int height)
    {
        m_RenderSize.x = width;
        m_RenderSize.y = height;

        m_GBuffer->Resize(width, height);

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
    void Renderer::SetTarget(RenderTarget* renderTarget, GLenum target)
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
    void Renderer::SetCamera(Camera* camera)
    {
        m_Camera = camera;
    }
    // ------------------------------------------------------------------------
    PostProcessor* Renderer::GetPostProcessor()
    {
        return m_PostProcessor;
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
    void Renderer::PushRender(Mesh* mesh, Material* material, math::mat4 transform, math::mat4 prevFrameTransform)
    {
        // get current render target
        RenderTarget* target = getCurrentRenderTarget();
        // don't render right away but push to the command buffer for later rendering.
        m_CommandBuffer->Push(mesh, material, transform, prevFrameTransform, math::vec3(-99999.0f), math::vec3(99999.0f), target);
    }
    // ------------------------------------------------------------------------
    void Renderer::PushRender(SceneNode* node)
    {
        // update transform(s) before pushing node to render command buffer
        node->UpdateTransform(true);

        // get current render target
        RenderTarget* target = getCurrentRenderTarget();
        // traverse through all the scene nodes and for each node: push its render state to the 
        // command buffer together with a calculated transform matrix.
        std::stack<SceneNode*> nodeStack;
        nodeStack.push(node);
        for (unsigned int i = 0; i < node->GetChildCount(); ++i)
            nodeStack.push(node->GetChildByIndex(i));
        while (!nodeStack.empty())
        {
            SceneNode* node = nodeStack.top();
            nodeStack.pop();
            // only push render command if the child isn't a container node.
            if (node->Mesh)
            {
                math::vec3 boxMinWorld = node->GetWorldPosition() + (node->GetWorldScale() * node->BoxMin);
                math::vec3 boxMaxWorld = node->GetWorldPosition() + (node->GetWorldScale() * node->BoxMax);
                 m_CommandBuffer->Push(node->Mesh, node->Material, node->GetTransform(), node->GetPrevTransform(), boxMinWorld, boxMaxWorld, target);
            }
            for(unsigned int i = 0; i < node->GetChildCount(); ++i)
                nodeStack.push(node->GetChildByIndex(i));
        }
    }
    // ------------------------------------------------------------------------
    void Renderer::PushPostProcessor(Material* postProcessor)
    {
        // we only care about the material, mesh as NDC quad is pre-defined.
        m_CommandBuffer->Push(nullptr, postProcessor);
    }
    // ------------------------------------------------------------------------
    void Renderer::AddLight(DirectionalLight* light)
    {
        m_DirectionalLights.push_back(light);
    }
    // ------------------------------------------------------------------------
    void Renderer::AddLight(PointLight* light)
    {
        m_PointLights.push_back(light);
    }
    // ------------------------------------------------------------------------
    void Renderer::RenderPushedCommands()
    {      
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 
        
          General outline of all the render steps/passes:
          - First we render all pushed geometry to the GBuffer.
          - We then render all shadow casting geometry to the shadow buffer.
          - Pre-lighting post-processing steps.
          - Deferred lighting pass (ambient, directional, point).
          - Process deferred data s.t. forward pass is neatly integrated with deferred pass.
          - Then we do the forward 'custom' rendering pass where developers can write their 
            own shaders and render stuff as they want, not sacrifcing flexibility; this 
            includes custom render targets.
          - Then we render all alpha blended materials last.
          - Then we do post-processing, one can supply their own post-processing materials 
            by setting the 'post-processing' flag of the material.
            Each material flagged as post-processing will run after the default post-processing 
            shaders (before/after HDR-tonemap/gamma-correct).

        */
        // sort all pushed render commands by heavy state-switches e.g. shader switches.
        m_CommandBuffer->Sort();

        // update (global) uniform buffers
        updateGlobalUBOs();

        // set default GL state
        m_GLCache.SetBlend(false);
        m_GLCache.SetCull(true);
        m_GLCache.SetCullFace(GL_BACK);
        m_GLCache.SetDepthTest(true);
        m_GLCache.SetDepthFunc(GL_LESS);

        // 1. Geometry buffer
        std::vector<RenderCommand> deferredRenderCommands = m_CommandBuffer->GetDeferredRenderCommands(true);
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer->ID);
        unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(4, attachments);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        m_GLCache.SetPolygonMode(Wireframe ? GL_LINE : GL_FILL);
        for (unsigned int i = 0; i < deferredRenderCommands.size(); ++i)
        {
            renderCustomCommand(&deferredRenderCommands[i], nullptr, false);
        }
        m_GLCache.SetPolygonMode(GL_FILL);

        //attachments[0] = GL_NONE; // disable for next pass (shadow map generation)
        attachments[1] = GL_NONE;
        attachments[2] = GL_NONE;
        attachments[3] = GL_NONE;
        glDrawBuffers(4, attachments);

        // 2. render all shadow casters to light shadow buffers
        if (Shadows)
        {
            m_GLCache.SetCullFace(GL_FRONT);
            std::vector<RenderCommand> shadowRenderCommands = m_CommandBuffer->GetShadowCastRenderCommands();
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
            m_GLCache.SetCullFace(GL_BACK);
        }
        attachments[0] = GL_COLOR_ATTACHMENT0;
        glDrawBuffers(4, attachments);

        // 3. do post-processing steps before lighting stage (e.g. SSAO)
        m_PostProcessor->ProcessPreLighting(this, m_GBuffer, m_Camera);

        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
        glViewport(0, 0, m_CustomTarget->Width, m_CustomTarget->Height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 4. Render deferred shader for each light (full quad for directional, spheres for point lights)
        m_GLCache.SetDepthTest(false);
        m_GLCache.SetBlend(true);
        m_GLCache.SetBlendFunc(GL_ONE, GL_ONE);

        // bind gbuffer
        m_GBuffer->GetColorTexture(0)->Bind(0);
        m_GBuffer->GetColorTexture(1)->Bind(1);
        m_GBuffer->GetColorTexture(2)->Bind(2);
        
        // ambient lighting
        renderDeferredAmbient();

        if (Lights)
        {
            // directional lights
            for (auto it = m_DirectionalLights.begin(); it != m_DirectionalLights.end(); ++it)
            {
                renderDeferredDirLight(*it);
            }
            // point lights
            m_GLCache.SetCullFace(GL_FRONT);
            for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
            {
                // only render point lights if within frustum
                if (m_Camera->Frustum.Intersect((*it)->Position, (*it)->Radius))
                {
                    renderDeferredPointLight(*it);
                }
            }
            m_GLCache.SetCullFace(GL_BACK);
        }

        m_GLCache.SetDepthTest(true);
        m_GLCache.SetBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        m_GLCache.SetBlend(false);

        // 5. blit depth buffer to default for forward rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_CustomTarget->ID); // write to default framebuffer
        glBlitFramebuffer(
            0, 0, m_GBuffer->Width, m_GBuffer->Height, 0, 0, m_RenderSize.x, m_RenderSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

        // 6. custom forward render pass 
        // push default render target to the end of the render target buffer s.t. we always render 
        // the default buffer last.
        m_RenderTargetsCustom.push_back(nullptr);        
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
                m_Camera->SetPerspective(m_Camera->FOV, 
                                         (float)renderTarget->Width / (float)renderTarget->Height, 
                                         0.1, 100.0f); 
            }
            else
            {
                // don't render to default framebuffer, but to custom target framebuffer which 
                // we'll use for post-processing.
                glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
                glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
                m_Camera->SetPerspective(m_Camera->FOV, m_RenderSize.x / m_RenderSize.y, 0.1, 
                                         100.0f);
            }

            // sort all render commands and retrieve the sorted array
            std::vector<RenderCommand> renderCommands = m_CommandBuffer->GetCustomRenderCommands(renderTarget);

            // terate over all the render commands and execute
            m_GLCache.SetPolygonMode(Wireframe ? GL_LINE : GL_FILL);
            for (unsigned int i = 0; i < renderCommands.size(); ++i)
            {
                renderCustomCommand(&renderCommands[i], nullptr);
            }
            m_GLCache.SetPolygonMode(GL_FILL);
        }

        // 7. alpha material pass
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
        std::vector<RenderCommand> alphaRenderCommands = m_CommandBuffer->GetAlphaRenderCommands(true);
        for (unsigned int i = 0; i < alphaRenderCommands.size(); ++i)
        {
            renderCustomCommand(&alphaRenderCommands[i], nullptr);
        }

        // render light mesh (as visual cue), if requested
        for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
        {
            if ((*it)->RenderMesh)
            {
                m_MaterialLibrary->debugLightMaterial->SetVector("lightColor", (*it)->Color * (*it)->Intensity * 0.25f);

                RenderCommand command;
                command.Material = m_MaterialLibrary->debugLightMaterial;
                command.Mesh = m_DebugLightMesh;
                math::mat4 model;
                math::translate(model, (*it)->Position);
                math::scale(model, math::vec3(0.25f));
                command.Transform = model;

                renderCustomCommand(&command, nullptr);
            }
        }

        // 8. post-processing stage after all lighting calculations 
        m_PostProcessor->ProcessPostLighting(this, m_GBuffer, m_CustomTarget, m_Camera);

        // 9. render debug visuals
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->ID);
        if (LightVolumes)
        {
            m_GLCache.SetPolygonMode(GL_LINE);
            m_GLCache.SetCullFace(GL_FRONT);
            for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
            {
                m_MaterialLibrary->debugLightMaterial->SetVector("lightColor", (*it)->Color);

                RenderCommand command;
                command.Material = m_MaterialLibrary->debugLightMaterial;
                command.Mesh = m_DebugLightMesh;
                math::mat4 model;
                math::translate(model, (*it)->Position);
                math::scale(model, math::vec3((*it)->Radius));
                command.Transform = model;

                renderCustomCommand(&command, nullptr);
            }
            m_GLCache.SetPolygonMode(GL_FILL);
            m_GLCache.SetCullFace(GL_BACK);
        }
        if (RenderProbes)
        {
            m_PBR->RenderProbes();
        }

        // 10. custom post-processing pass
        std::vector<RenderCommand> postProcessingCommands = m_CommandBuffer->GetPostProcessingRenderCommands();
        for (unsigned int i = 0; i < postProcessingCommands.size(); ++i)
        {
            // ping-pong between render textures
            bool even = i % 2 == 0;
            Blit(even ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0),
                 even ? m_PostProcessTarget1 : m_CustomTarget, 
                 postProcessingCommands[i].Material);
        }

        // 11. final post-processing steps, blitting to default framebuffer
        m_PostProcessor->Blit(this, postProcessingCommands.size() % 2 == 0 ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0));

        // store view projection as previous view projection for next frame's motion blur
        m_PrevViewProjection = m_Camera->Projection * m_Camera->View;

        // clear the command buffer s.t. the next frame/call can start from an empty slate again.
        m_CommandBuffer->Clear();

        // clear render state
        m_RenderTargetsCustom.clear();
        m_CurrentRenderTargetCustom = nullptr;
    }
    // ------------------------------------------------------------------------
    void Renderer::Blit(Texture*      src,
                        RenderTarget* dst, 
                        Material*     material, 
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
        scene->UpdateTransform();
        // build a command list of nodes within the reflection probe's capture box/radius.
        CommandBuffer commandBuffer(this);
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
                    materials.push_back(new Material(m_PBR->m_ProbeCaptureBackgroundShader));
                    materials[materials.size() - 1]->SetTextureCube("background", samplerUniforms["background"].TextureCube, 0);
                    materials[materials.size() - 1]->DepthCompare = node->Material->DepthCompare;
                    commandBuffer.Push(node->Mesh, materials[materials.size() - 1], node->GetTransform());
                }
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
    void Renderer::renderCustomCommand(RenderCommand* command, Camera* customCamera, bool updateGLSettings)
    {
        Material *material = command->Material;
        Mesh     *mesh     = command->Mesh;

        // update global GL blend state based on material
        if (updateGLSettings)
        {
            m_GLCache.SetBlend(material->Blend);
            if(material->Blend)
            {
                m_GLCache.SetBlendFunc(material->BlendSrc, material->BlendDst);
            }
            m_GLCache.SetDepthFunc(material->DepthCompare);
            m_GLCache.SetDepthTest(material->DepthTest);
            m_GLCache.SetCull(material->Cull);
            m_GLCache.SetCullFace(material->CullFace);
        }

        // default uniforms that are always configured regardless of shader configuration (see them 
        // as a default set of shader uniform variables always there); with UBO
        material->GetShader()->Use();
        if (customCamera) // pass custom camera specific uniform
        {
            material->GetShader()->SetMatrix("projection", customCamera->Projection);
            material->GetShader()->SetMatrix("view",       customCamera->View);
            material->GetShader()->SetVector("CamPos",     customCamera->Position);
        }
        material->GetShader()->SetMatrix("model", command->Transform);
        material->GetShader()->SetMatrix("prevModel", command->PrevTransform);

        material->GetShader()->SetBool("ShadowsEnabled", Shadows);
        if (Shadows && material->Type == MATERIAL_CUSTOM && material->ShadowReceive)
        {
            for (int i = 0; i < m_DirectionalLights.size(); ++i)
            {
                if (m_DirectionalLights[i]->ShadowMapRT)
                {
                    material->GetShader()->SetMatrix("lightShadowViewProjection" + std::to_string(i + 1), m_DirectionalLights[i]->LightSpaceViewProjection);
                    m_DirectionalLights[i]->ShadowMapRT->GetDepthStencilTexture()->Bind(10 + i);
                }
            }
        }

        // bind/active uniform sampler/texture objects
        auto* samplers = material->GetSamplerUniforms();
        for (auto it = samplers->begin(); it != samplers->end(); ++it)
        {
            if (it->second.Type == SHADER_TYPE_SAMPLERCUBE)
                it->second.TextureCube->Bind(it->second.Unit);
            else
                it->second.Texture->Bind(it->second.Unit);
        }

        // set uniform state of material
        auto* uniforms = material->GetUniforms();
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
    void Renderer::renderToCubemap(SceneNode* scene,
        TextureCube* target,
        math::vec3   position,
        unsigned int mipLevel)
    {
        // create a command buffer specifically for this operation (as to not conflict with main 
        // command buffer)
        CommandBuffer commandBuffer(this);
        commandBuffer.Push(scene->Mesh, scene->Material, scene->GetTransform());
        // recursive function transformed to iterative version by maintaining a stack
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
        glBindBuffer(GL_UNIFORM_BUFFER, m_GlobalUBO);
        // transformation matrices
        glBufferSubData(GL_UNIFORM_BUFFER,   0, sizeof(math::mat4), &(m_Camera->Projection * m_Camera->View)[0][0]); // sizeof(math::mat4) = 64 bytes
        glBufferSubData(GL_UNIFORM_BUFFER,  64, sizeof(math::mat4), &m_PrevViewProjection[0][0]); 
        glBufferSubData(GL_UNIFORM_BUFFER, 128, sizeof(math::mat4), &m_Camera->Projection[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 192, sizeof(math::mat4), &m_Camera->View[0][0]);
        glBufferSubData(GL_UNIFORM_BUFFER, 256, sizeof(math::mat4), &m_Camera->View[0][0]); // TODO: make inv function in math library
        // scene data
        glBufferSubData(GL_UNIFORM_BUFFER, 320, sizeof(math::vec4), &m_Camera->Position[0]);
        // lighting
        unsigned int stride = 2 * sizeof(math::vec4);
        for (unsigned int i = 0; i < m_DirectionalLights.size() && i < 4; ++i) // no more than 4 directional lights
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 336 + i * stride,                      sizeof(math::vec4), &m_DirectionalLights[i]->Direction[0]);
            glBufferSubData(GL_UNIFORM_BUFFER, 336 + i * stride + sizeof(math::vec4), sizeof(math::vec4), &m_DirectionalLights[i]->Color[0]);
        }
        for (unsigned int i = 0; i < m_PointLights.size() && i < 8; ++i) //  constrained to max 8 point lights in forward context
        {
            glBufferSubData(GL_UNIFORM_BUFFER, 464 + i * stride,                      sizeof(math::vec4), &m_PointLights[i]->Position[0]);
            glBufferSubData(GL_UNIFORM_BUFFER, 464 + i * stride + sizeof(math::vec4), sizeof(math::vec4), &m_PointLights[i]->Color[0]);
        }
    }
    // --------------------------------------------------------------------------------------------
    RenderTarget* Renderer::getCurrentRenderTarget()
    {
        return m_CurrentRenderTargetCustom;
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredAmbient()
    {
        PBRCapture* skyCapture = m_PBR->GetSkyCapture();
        auto irradianceProbes = m_PBR->m_CaptureProbes;

        // if irradiance probes are present, use these as ambient lighting
        if (IrradianceGI && irradianceProbes.size() > 0)
        {
            skyCapture->Prefiltered->Bind(4);
            m_PBR->m_RenderTargetBRDFLUT->GetColorTexture(0)->Bind(5);
            m_PostProcessor->SSAOOutput->Bind(6);

            m_GLCache.SetCullFace(GL_FRONT);
            for (int i = 0; i < irradianceProbes.size(); ++i)
            {
                PBRCapture* probe = irradianceProbes[i];
                // only render probe if within frustum
                if (m_Camera->Frustum.Intersect(probe->Position, probe->Radius))
                {
                    probe->Irradiance->Bind(3);

                    Shader* irradianceShader = m_MaterialLibrary->deferredIrradianceShader;
                    irradianceShader->Use();
                    irradianceShader->SetVector("camPos", m_Camera->Position);
                    irradianceShader->SetVector("probePos", probe->Position);
                    irradianceShader->SetFloat("probeRadius", probe->Radius);
                    irradianceShader->SetInt("SSAO", m_PostProcessor->SSAO);

                    math::mat4 model;
                    math::translate(model, probe->Position);
                    math::scale(model, math::vec3(probe->Radius));
                    irradianceShader->SetMatrix("model", model);

                    renderMesh(m_DeferredPointMesh, irradianceShader);
                }
            }
            m_GLCache.SetCullFace(GL_BACK);
        }
        // otherwise do a full-screen ambient pass
        else
        {
            skyCapture->Irradiance->Bind(3);
            skyCapture->Prefiltered->Bind(4);
            m_PBR->m_RenderTargetBRDFLUT->GetColorTexture(0)->Bind(5);
            m_PostProcessor->SSAOOutput->Bind(6);

            Shader* ambientShader = m_MaterialLibrary->deferredAmbientShader;
            ambientShader->Use();
            ambientShader->SetInt("SSAO", m_PostProcessor->SSAO);
            renderMesh(m_NDCPlane, ambientShader);
        }
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredDirLight(DirectionalLight* light)
    {
        Shader* dirShader = m_MaterialLibrary->deferredDirectionalShader;

        dirShader->Use();
        dirShader->SetVector("camPos", m_Camera->Position);
        dirShader->SetVector("lightDir", light->Direction);
        dirShader->SetVector("lightColor", math::normalize(light->Color) * light->Intensity); 
        dirShader->SetBool("ShadowsEnabled", Shadows);

        if (light->ShadowMapRT)
        {
            dirShader->SetMatrix("lightShadowViewProjection", light->LightSpaceViewProjection);
            light->ShadowMapRT->GetDepthStencilTexture()->Bind(3);
        }
            
        renderMesh(m_NDCPlane, dirShader);
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredPointLight(PointLight* light)
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