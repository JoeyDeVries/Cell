#include "renderer.h"

#include "render_target.h"
#include "MaterialLibrary.h"

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


//void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam);

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

        // lighting
        delete m_DebugLightMesh;

        // post-processing
        delete m_PostProcessTarget1;

        // pbr
        delete m_PBRCaptureCube;
        delete m_TargetBRDFLUT;
        delete m_PBRHdrToCubemap;
        delete m_PBRIrradianceCapture;
        delete m_PBRPrefilterCapture;
        delete m_PBRIntegrateBRDF;
        for (unsigned int i = 0; i < m_PBREnvironments.size(); ++i)
        {
            delete m_PBREnvironments[i]->Irradiance;
            delete m_PBREnvironments[i]->Prefiltered;
            delete m_PBREnvironments[i];
        }
    }
    // ------------------------------------------------------------------------
    void Renderer::Init(GLADloadproc loadProcFunc)
    {
        // NOTE(Joey): pre-compute PBR 
        // ---

        // NOTE(Joey): initialize render items
        // TODO(Joey): do we want to abstract this or not? as it is very specific.
        m_NDCPlane = new Quad;
        glGenFramebuffers(1, &m_FramebufferCubemap);
        glGenRenderbuffers(1, &m_CubemapDepthRBO);

        m_CustomTarget = new RenderTarget(1, 1, GL_HALF_FLOAT, 1, true);
        m_PostProcessTarget1 = new RenderTarget(1, 1, GL_UNSIGNED_BYTE, 1, false);

        // lights
        m_DebugLightMesh = new Sphere(16, 16);
        m_DeferredPointMesh = new Sphere(16, 16);


        // deferred renderer
        m_GBuffer = new RenderTarget(1, 1, GL_HALF_FLOAT, 3, true);

        // materials
        m_MaterialLibrary = new MaterialLibrary(m_GBuffer);       
       
        m_PBRCaptureCube = new Cube();
        m_TargetBRDFLUT = new RenderTarget(128, 128, GL_HALF_FLOAT, 1, true);
        Cell::Shader *hdrToCubemap      = Cell::Resources::LoadShader("pbr:hdr to cubemap", "shaders/pbr/cube_sample.vs", "shaders/pbr/spherical_to_cube.fs");
        Cell::Shader *irradianceCapture = Cell::Resources::LoadShader("pbr:irradiance",     "shaders/pbr/cube_sample.vs", "shaders/pbr/irradiance_capture.fs");
        Cell::Shader *prefilterCapture  = Cell::Resources::LoadShader("pbr:prefilter",      "shaders/pbr/cube_sample.vs", "shaders/pbr/prefilter_capture.fs");
        Cell::Shader *integrateBrdf     = Cell::Resources::LoadShader("pbr:integrate_brdf", "shaders/screen_quad.vs",     "shaders/pbr/integrate_brdf.fs");
        m_PBRHdrToCubemap      = new Material(hdrToCubemap);
        m_PBRIrradianceCapture = new Material(irradianceCapture);
        m_PBRPrefilterCapture  = new Material(prefilterCapture);
        m_PBRIntegrateBRDF     = new Material(integrateBrdf);
        m_PBRHdrToCubemap->DepthCompare      = GL_LEQUAL;
        m_PBRIrradianceCapture->DepthCompare = GL_LEQUAL;
        m_PBRPrefilterCapture->DepthCompare  = GL_LEQUAL;

        // - brdf integration
        Blit(nullptr, m_TargetBRDFLUT, m_PBRIntegrateBRDF);
        //for (auto it = m_DefaultMaterials.begin(); it != m_DefaultMaterials.end(); ++it)
        //{
        //    it->second->SetTexture("BRDFLUT", m_TargetBRDFLUT->GetColorTexture(0), 0);
        //}

        // NOTE(Joey): default PBR pre-compute (get a more default oriented HDR map for this)
        Cell::Texture *hdrMap = Cell::Resources::LoadHDR("hdr factory catwalk", "textures/backgrounds/hamarikyu bridge.hdr");
        Cell::PBREnvironment *envBridge = PBREnvMapPrecompute(hdrMap);
        SetPBREnvironment(envBridge);
    }
    // ------------------------------------------------------------------------
    void Renderer::SetRenderSize(unsigned int width, unsigned int height)
    {
        m_RenderSize.x = width;
        m_RenderSize.y = height;

        m_CustomTarget->Resize(width, height);
        m_PostProcessTarget1->Resize(width, height);
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
        // NOTE(Joey): get current render target
        RenderTarget *target = getCurrentRenderTarget();
        // NOTE(Joey): don't render right away but push to the command buffer
        // for later rendering.
        m_CommandBuffer.Push(mesh, material, transform, target);
    }
    // ------------------------------------------------------------------------
    void Renderer::PushRender(SceneNode *node)
    {
        // NOTE(Joey): get current render target
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
            for(unsigned int i = 0; i < child->GetChildCount(); ++i)
                childStack.push(child->GetChildByIndex(i));
        }
    }
    // ------------------------------------------------------------------------
    void Renderer::PushPostProcessor(Material *postProcessor)
    {
        // NOTE(Joey): we only care about the material, mesh as NDC quad is pre-defined.
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

        // NOTE(Joey): begin with a forward renderer context for rendering of
        // the command buffer, later switch to a deferred render pipeline once
        // all sub-systems in place work properly.
        m_CommandBuffer.Sort();

        // NOTE(Joey); deferred here:
        std::vector<RenderCommand> deferredRenderCommands = m_CommandBuffer.GetDeferredRenderCommands();
        // 1. Geometry buffer
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
        glBindFramebuffer(GL_FRAMEBUFFER, m_GBuffer->m_ID);
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
        glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->m_ID);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // 2. Render deferred shader for each light (full quad for directional, spheres for point lights)
        // ambient lighting
        renderDeferredAmbient();

        // directional lights
        for (auto it = m_DirectionalLights.begin(); it != m_DirectionalLights.end(); ++it)
        {
            renderDeferredDirLight(*it);
        }
        // point lights
        glCullFace(GL_FRONT);
        for (auto it = m_PointLights.begin(); it != m_PointLights.end(); ++it)
        {
            renderDeferredPointLight(*it);
        }
        glCullFace(GL_BACK);

        attachments[1] = GL_NONE;
        attachments[2] = GL_NONE;
        glDrawBuffers(3, attachments);

        // 3. blit depth buffer to default for forward rendering
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_GBuffer->m_ID);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_CustomTarget->m_ID); // write to default framebuffer
        glBlitFramebuffer(
            0, 0, m_GBuffer->Width, m_GBuffer->Height, 0, 0, m_RenderSize.x, m_RenderSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );

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
                glBindFramebuffer(GL_FRAMEBUFFER, renderTarget->m_ID);
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
                glBindFramebuffer(GL_FRAMEBUFFER, m_CustomTarget->m_ID);
                m_Camera->SetPerspective(m_Camera->FOV, m_RenderSize.x / m_RenderSize.y, 0.1, 
                                         100.0f);
            }

            // NOTE(Joey): sort all render commands and retrieve the sorted array
            std::vector<RenderCommand> renderCommands = m_CommandBuffer.GetCustomRenderCommands(renderTarget);

            // NOTE(Joey): iterate over all the render commands and execute
            for (unsigned int i = 0; i < renderCommands.size(); ++i)
            {
                renderCustomCommand(&renderCommands[i], m_Camera);
            }
        }

        // render (debug) visuals
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

        // NOTE(Joey): get combined color/depth texture of deferred/custom pass


        // NOTE(Joey): then do post-processing
        std::vector<RenderCommand> postProcessingCommands = m_CommandBuffer.GetPostProcessingRenderCommands();
        for (unsigned int i = 0; i < postProcessingCommands.size(); ++i)
        {
            // NOTE(Joey): ping-pong between render textures
            bool even = i % 2 == 0;
            Blit(even ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0),
                 even ? m_PostProcessTarget1 : m_CustomTarget, 
                 postProcessingCommands[i].Material);
        }

        // NOTE(Joey): then do a final blit to the default framebuffer, with gamma correction and 
        // tone mapping post-processing step.
        Blit(postProcessingCommands.size() % 2 == 0 ? m_CustomTarget->GetColorTexture(0) : m_PostProcessTarget1->GetColorTexture(0),
             nullptr, m_MaterialLibrary->postProcessingMaterial);

        //Blit(m_GBuffer->GetColorTexture(0));

        // NOTE(Joey): clear the command buffer s.t. the next frame/call can
        // start from an empty slate again.
        m_CommandBuffer.Clear();

        // NOTE(Joey): clear render state
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
        // NOTE(Joey): if a destination target is given, bind to its framebuffer
        if (dst)
        {
            glViewport(0, 0, dst->Width, dst->Height);
            glBindFramebuffer(GL_FRAMEBUFFER, dst->m_ID);
            if (dst->HasDepthAndStencil)
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            else
                glClear(GL_COLOR_BUFFER_BIT);
        }
        // NOTE(Joey): else we bind to the default framebuffer
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        }
        // if no material is given, use default blit material
        if (!material)
        {
            material = m_MaterialLibrary->defaultBlitMaterial;
        }
        // NOTE(Joey): if a source render target is given, use its color buffer
        // as input to the material shader.
        if (src)
        {
            material->SetTexture(textureUniformName, src, 0);
        }
        // NOTE(Joey): render screen-space material to quad which will be 
        // stored/displayer inside dst's buffers.
        RenderCommand command;
        command.Material = material;
        command.Mesh = m_NDCPlane;
        renderCustomCommand(&command, nullptr);

        // NOTE(Joey): revert back to default framebuffer
        // NOTE(Joey): is this necessary? do we want this?
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y); 
    }
    // ------------------------------------------------------------------------
    void Renderer::RenderToCubemap(SceneNode *  scene, 
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

        // NOTE(Joey): define 6 camera directions/lookup vectors
        Camera faceCameras[6] = {
            Camera(position, math::vec3( 1.0f,  0.0f,  0.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3(-1.0f,  0.0f,  0.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3( 0.0f,  1.0f,  0.0f), math::vec3(0.0f,  0.0f,  1.0f)),
            Camera(position, math::vec3( 0.0f, -1.0f,  0.0f), math::vec3(0.0f,  0.0f,- 1.0f)),
            Camera(position, math::vec3( 0.0f,  0.0f,  1.0f), math::vec3(0.0f, -1.0f,  0.0f)),
            Camera(position, math::vec3( 0.0f,  0.0f, -1.0f), math::vec3(0.0f, -1.0f,  0.0f))
        };

        // NOTE(Joey): resize target dimensions based on mip level we're rendering.
        float width = (float)target->FaceWidth * pow(0.5, mipLevel);
        float height = (float)target->FaceHeight * pow(0.5, mipLevel);

        // TODO(Joey): only resize/recreate if faceWidth is different than before
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);
        glBindRenderbuffer(GL_RENDERBUFFER, m_CubemapDepthRBO);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 
                                  m_CubemapDepthRBO);

        // NOTE(Joey): resize relevant buffers
        glViewport(0, 0, width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferCubemap);

        for (unsigned int i = 0; i < 6; ++i)
        {
            Camera *camera = &faceCameras[i];
            camera->SetPerspective(math::Deg2Rad(90.0f), width/height, 0.1f, 100.0f);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
                                   GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, target->ID, mipLevel);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            for (unsigned int i = 0; i < renderCommands.size(); ++i)
            {
                // NOTE(Joey): cubemap generation only works w/ custom materials (for now; not 
                // streamlined with deferred path yet)
                assert(renderCommands[i].Material->Type == MATERIAL_CUSTOM); 
                renderCustomCommand(&renderCommands[i], camera);
            }
        }
        // NOTE(Joey): reset state
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_RenderSize.x, m_RenderSize.y);
    }
    // ------------------------------------------------------------------------
    PBREnvironment* Renderer::PBREnvMapPrecompute(Texture *hdriEnvironment, math::vec3 location)
    {
        // TODO(Joey): memory management here is not cleanly organized, come up with better solution!
        PBREnvironment *result = new PBREnvironment;

        // NOTE(Joey): convert HDR radiance image to HDR environment cubemap
        Cell::SceneNode *environmentCube = Cell::Scene::MakeSceneNode(m_PBRCaptureCube, m_PBRHdrToCubemap);
        m_PBRHdrToCubemap->SetTexture("environment", hdriEnvironment, 0);
        // TODO(Joey): think of proper way to manage memory here.
        Cell::TextureCube hdrEnvMap;
        hdrEnvMap.DefaultInitialize(128, 128, GL_RGB, GL_FLOAT);
        RenderToCubemap(environmentCube, &hdrEnvMap);
        // - irradiance
        result->Irradiance = new TextureCube;
        result->Irradiance->DefaultInitialize(32, 32, GL_RGB, GL_FLOAT);
        m_PBRIrradianceCapture->SetTextureCube("environment", &hdrEnvMap, 0);
        environmentCube->Material = m_PBRIrradianceCapture;
        RenderToCubemap(environmentCube, result->Irradiance, math::vec3(0.0f), 0);
        // - prefilter 
        result->Prefiltered = new TextureCube;;
        result->Prefiltered->FilterMin = GL_LINEAR_MIPMAP_LINEAR;
        result->Prefiltered->DefaultInitialize(128, 128, GL_RGB, GL_FLOAT, true);
        m_PBRPrefilterCapture->SetTextureCube("environment", &hdrEnvMap, 0);
        environmentCube->Material = m_PBRPrefilterCapture;
        // calculate prefilter for multiple roughness levels
        unsigned int maxMipLevels = 5;
        for (unsigned int i = 0; i < maxMipLevels; ++i)
        {
            m_PBRPrefilterCapture->SetFloat("roughness", (float)i / (float)(maxMipLevels - 1));
            RenderToCubemap(environmentCube, result->Prefiltered, math::vec3(0.0f), i);

        }
        m_PBREnvironments.push_back(result);
        Scene::DeleteSceneNode(environmentCube);
        return result;
    }
    // ------------------------------------------------------------------------
    void Renderer::SetPBREnvironment(PBREnvironment *pbrEnvironment)
    {
        for(unsigned int i = 0; i < m_PBREnvironments.size(); ++i)
        {
            if (pbrEnvironment->Irradiance == m_PBREnvironments[i]->Irradiance)
            {
                m_PBREnvironmentIndex = i;
                // NOTE(Joey): loop through all the default shaders and set PBR environments
                // TODO(Joey): update UBO object, instead of setting uniforms like we do now.
                // TODO(Joey): manage this with pbr toolkit
                /*for (auto it = m_DefaultMaterials.begin(); it != m_DefaultMaterials.end(); ++it)
                {
                    it->second->SetTextureCube("EnvIrradiance", pbrEnvironment->Irradiance, 1);
                    it->second->SetTextureCube("EnvPrefilter", pbrEnvironment->Prefiltered, 2);
                }*/
                return;
            }
        }
        Log::Message("Tried to set PBR environment, but wasn't indexed before.");
    }
    // ------------------------------------------------------------------------
    PBREnvironment* Renderer::GetPBREnvironment()
    {
        return m_PBREnvironments[m_PBREnvironmentIndex];
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
        RenderCommand command;
        command.Material = m_MaterialLibrary->deferredAmbientMaterial;
        command.Mesh = m_NDCPlane;
        renderCustomCommand(&command, nullptr);
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredDirLight(DirectionalLight *light)
    {
        m_MaterialLibrary->deferredDirectionalMaterial->SetVector("lightDir", light->Direction);
        m_MaterialLibrary->deferredDirectionalMaterial->SetVector("lightColor", math::normalize(light->Color) * light->Intensity); // TODO(Joey): enforce light normalization with light setter?

        RenderCommand command;
        command.Material = m_MaterialLibrary->deferredDirectionalMaterial;
        command.Mesh = m_NDCPlane;
        renderCustomCommand(&command, m_Camera);
    }
    // --------------------------------------------------------------------------------------------
    void Renderer::renderDeferredPointLight(PointLight *light)
    {
        m_MaterialLibrary->deferredPointMaterial->SetVector("lightPos", light->Position);
        m_MaterialLibrary->deferredPointMaterial->SetVector("lightColor", math::normalize(light->Color) * light->Intensity);
        m_MaterialLibrary->deferredPointMaterial->SetFloat("lightRadius", light->Radius);

        RenderCommand command;
        command.Material = m_MaterialLibrary->deferredPointMaterial;
        command.Mesh = m_DeferredPointMesh;
        math::mat4 model;
        math::translate(model, light->Position);
        math::scale(model, math::vec3(light->Radius));
        command.Transform = model;

        renderCustomCommand(&command, m_Camera);
    }
}