#ifndef CELL_RENDERER_H
#define CELL_RENDERER_H

#include <math/linear_algebra/matrix.h>

#include "../lighting/point_light.h"
#include "../lighting/directional_light.h"
#include "../mesh/quad.h"
#include "command_buffer.h"
#include "pbr_environment.h"

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

    /* NOTE(Joey):

      The main renderer; responsible for maintaining a render buffer queue
      and providing the front push commands for filling the buffer, then
      sorting the buffer, manage multiple render passes and render the
      buffer accordingly.

    */
    class Renderer
    {
    private:
        CommandBuffer m_CommandBuffer;

        Camera       *m_Camera;

        // render state
        math::vec2 m_RenderSize;

        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<PointLight*>       m_PointLights;
        Mesh     *m_LightMesh;
        Material *m_LightMaterial;
        std::vector<RenderTarget*>  m_RenderTargetsCustom;
        RenderTarget               *m_CurrentRenderTargetCustom = nullptr;
        Quad                       *m_NDCPlane; 

        RenderTarget               *m_CustomTarget;

        // final post-processing
        RenderTarget *m_PostProcessTarget1;
        Material     *m_PostProcessingMaterial;

        // (dynamic) cubemap generation
        unsigned int m_FramebufferCubemap; // NOTE(Joey): cubemap render targets are a very specific case so we can do these without abstractions.
        unsigned int m_CubemapDepthRBO;

        // set of default materials 
        // NOTE(Joey): do we want to abstract this from the renderer?
        std::map<unsigned int, Material*> m_DefaultMaterials;
        std::vector<Material*> m_Materials; // NOTE(Joey): store generated materials here for easy memory management (TODO(Joey): think of cleaner way; same for scene!)

        // pbr
        std::vector<PBREnvironment*> m_PBREnvironments;
        unsigned int m_PBREnvironmentIndex;
        RenderTarget  *m_TargetBRDFLUT;
        Mesh     *m_PBRCaptureCube;
        Material *m_PBRHdrToCubemap;
        Material *m_PBRIrradianceCapture;
        Material *m_PBRPrefilterCapture;
        Material *m_PBRIntegrateBRDF;
    public:
        Renderer();
        ~Renderer();

        void Init(GLADloadproc loadProcFunc);

        void SetRenderSize(unsigned int width, unsigned int height);
        math::vec2 GetRenderSize();

        void SetTarget(RenderTarget *renderTarget, GLenum target = GL_TEXTURE_2D);

        Camera* GetCamera();
        void    SetCamera(Camera *camera);

        // NOTE(Joey): idea, create either a deferred default material (based on default set of materials available (like glass)), or a custom material (with custom you have to supply your own shader)
        Material* CreateMaterial(std::string base = "default"); // NOTE(Joey): these don't have the custom flag set (default material has default state and uses checkerboard texture as albedo (and black metallic, half roughness, purple normal, white ao)
        Material* CreateCustomMaterial(Shader *shader);         // NOTE(Joey): these have the custom flag set (will be rendered in forward pass)
        Material* CreatePostProcessingMaterial(Shader *shader); // NOTE(Joey): these have the post-processing flag set (will be rendered after deferred/forward pass)

        void PushRender(Mesh *mesh, Material *material, math::mat4 transform = math::mat4());
        void PushRender(SceneNode *node);
        void PushPostProcessor(Material *postProcessor);

        void PushLight(DirectionalLight *light);
        void PushLight(PointLight       *light, bool render = false);        

        void RenderPushedCommands();

        void Blit(RenderTarget *src, RenderTarget *dst, Material *material, std::string textureUniformName = "TexSrc");
        void RenderToCubemap(SceneNode *scene, TextureCube *target, math::vec3 position = math::vec3(0.0f), unsigned int mipLevel = 0);
        PBREnvironment* PBREnvMapPrecompute(Texture *hdriEnvironment, math::vec3 location = math::vec3(0.0f));
        void SetPBREnvironment(PBREnvironment *pbrEnvironment);
        PBREnvironment* GetPBREnvironment();
    private:
        void renderCustomCommand(RenderCommand *command, Camera *camera);
        RenderTarget *getCurrentRenderTarget();
    };
}

#endif