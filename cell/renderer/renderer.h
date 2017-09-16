#ifndef CELL_RENDERER_H
#define CELL_RENDERER_H

#include <math/linear_algebra/matrix.h>

#include "../lighting/point_light.h"
#include "../lighting/directional_light.h"
#include "../mesh/quad.h"
#include "command_buffer.h"
#include "pbr_capture.h"

#include "../glad/glad.h"

namespace Cell
{
    /* NOTE(Joey):

      Forward declare the types, s.t. we don't need to include the
      required header files here. Seeing as most objects throughout Cell will
      link to renderer.h we want to reduce as much unnecesary additional
      header code as much as possible (saves in compilation times). And seeing
      as we only need the typename for the function specifications this works
      just fine.

    */
    class Mesh;
    class Material;
    class Scene;
    class SceneNode;
    class Camera;
    class RenderTarget;
    class MaterialLibrary;
    class PBR;
    class PostProcessor;

    /* NOTE(Joey):

      The main renderer; responsible for maintaining a render buffer queue
      and providing the front push commands for filling the buffer, then
      sorting the buffer, manage multiple render passes and render the
      buffer accordingly.

    */
    class Renderer
    {
        friend PostProcessor;
    private:
        // render state
        CommandBuffer m_CommandBuffer;
        math::vec2 m_RenderSize;

        // lighting
        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<PointLight*>       m_PointLights;
        RenderTarget* m_GBuffer = nullptr;
        Mesh*         m_DeferredPointMesh;

        // materials
        MaterialLibrary *m_MaterialLibrary;

        // camera
        Camera       *m_Camera;

        // render-targets/post
        std::vector<RenderTarget*> m_RenderTargetsCustom;
        RenderTarget*              m_CurrentRenderTargetCustom = nullptr;
        RenderTarget*              m_CustomTarget;
        RenderTarget*              m_PostProcessTarget1;
        PostProcessor*             m_PostProcessor;
        Quad*                      m_NDCPlane;
        unsigned int m_FramebufferCubemap; // NOTE(Joey): cubemap render targets are a very specific case so we can do these without abstractions.
        unsigned int m_CubemapDepthRBO;

        // shadow buffers
        std::vector<RenderTarget*> m_ShadowRenderTargets;
        std::vector<math::mat4>    m_ShadowViewProjections;
       
        // pbr
        PBR* m_PBR;
        unsigned int m_PBREnvironmentIndex;

        // debug
        Mesh *m_DebugLightMesh;

    public:
        Renderer();
        ~Renderer();

        void Init(GLADloadproc loadProcFunc);

        void SetRenderSize(unsigned int width, unsigned int height);
        math::vec2 GetRenderSize();

        void SetTarget(RenderTarget* renderTarget, GLenum target = GL_TEXTURE_2D);

        Camera* GetCamera();
        void    SetCamera(Camera* camera);

        // NOTE(Joey): idea, create either a deferred default material (based on default set of materials available (like glass)), or a custom material (with custom you have to supply your own shader)
        Material* CreateMaterial(std::string base = "default"); // NOTE(Joey): these don't have the custom flag set (default material has default state and uses checkerboard texture as albedo (and black metallic, half roughness, purple normal, white ao)
        Material* CreateCustomMaterial(Shader* shader);         // NOTE(Joey): these have the custom flag set (will be rendered in forward pass)
        Material* CreatePostProcessingMaterial(Shader* shader); // NOTE(Joey): these have the post-processing flag set (will be rendered after deferred/forward pass)

        void PushRender(Mesh* mesh, Material* material, math::mat4 transform = math::mat4());
        void PushRender(SceneNode* node);
        void PushPostProcessor(Material* postProcessor);

        void AddLight(DirectionalLight *light);
        void AddLight(PointLight       *light);        

        void RenderPushedCommands();

        void Blit(Texture* src, RenderTarget* dst = nullptr, Material* material = nullptr, std::string textureUniformName = "TexSrc");
        void RenderToCubemap(SceneNode* scene, TextureCube* target, math::vec3 position = math::vec3(0.0f), unsigned int mipLevel = 0);
        void SetPBREnvironment(PBRCapture* pbrEnvironment);
        PBRCapture* GetPBREnvironment();
    private:
        // renderer-specific logic for rendering a 'default' deferred command.
        void renderDeferredCommand(RenderCommand *command, Camera *camera);
        // renderer-specific logic for rendering a custom (forward-pass) command
        void renderCustomCommand(RenderCommand *command, Camera *camera);
        // minimal render logic to render a mesh 
        void renderMesh(Mesh* mesh, Shader* shader);
        // updates the global uniform buffer objects
        void updateGlobalUBOs();
        // returns the currently active render target
        RenderTarget *getCurrentRenderTarget();

        // deferred logic:
        // renders all ambient lighting (including indirect IBL)
        void renderDeferredAmbient();
        // render directional light
        void renderDeferredDirLight(DirectionalLight *light);
        // render point light
        void renderDeferredPointLight(PointLight *light);
        // render spot light

        // render mesh for shadow buffer generation
        void renderShadowCastCommand(RenderCommand* command, const math::mat4& projection, const math::mat4& view);

        // render (debug) mesh at light position

    };
}

#endif