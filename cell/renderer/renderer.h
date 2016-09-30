#ifndef CELL_RENDERER_H
#define CELL_RENDERER_H

#include <math/linear_algebra/matrix.h>

#include "../lighting/point_light.h"
#include "../lighting/directional_light.h"
#include "command_buffer.h"

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
        std::vector<DirectionalLight*> m_DirectionalLights;
        std::vector<PointLight*>       m_PointLights;
        Mesh     *m_LightMesh;
        Material *m_LightMaterial;
        std::vector<RenderTarget*>  m_RenderTargetsCustom;
        RenderTarget               *m_CurrentRenderTargetCustom = nullptr;

        // (dynamic) cubemap generation
        unsigned int m_FramebufferCubemap; // NOTE(Joey): cubemap render targets are a very specific case so we can do these without abstractions.
        unsigned int m_CubemapDepthRBO;
    public:
        Renderer();
        ~Renderer();

        void Init();

        void SetTarget(RenderTarget *renderTarget, GLenum target = GL_TEXTURE_2D);

        Camera* GetCamera();
        void    SetCamera(Camera *camera);

        void PushRender(Mesh *mesh, Material *material, math::mat4 transform = math::mat4());
        void PushRender(SceneNode *node);
        void PushRender(Scene *scene);

        void PushLight(DirectionalLight *light);
        void PushLight(PointLight       *light, bool render = false);        

        void RenderPushedCommands();

        void RenderToCubemap(SceneNode *scene, TextureCube *target, math::vec3 position, unsigned int mipLevel = 0);
    private:
        void renderCustomCommand(RenderCommand *command, Camera *camera);
        RenderTarget *getCurrentRenderTarget();
    };
}

#endif