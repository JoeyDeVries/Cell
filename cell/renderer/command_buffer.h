#ifndef CELL_RENDERER_COMMAND_BUFFER_H
#define CELL_RENDERER_COMMAND_BUFFER_H

#include "render_command.h"

#include <math/linear_algebra/matrix.h>

#include <map>
#include <vector>

namespace Cell
{
    class Renderer;
    class Mesh;
    class Material;
    class RenderTarget;

    /* 

      Render command buffer, managing all per-frame render/draw calls and converting them to a 
      (more efficient) render-friendly format for the renderer to execute.

    */
    class CommandBuffer
    {
    public:

    private:
        Renderer* m_Renderer;

        std::vector<RenderCommand> m_DeferredRenderCommands;
        std::vector<RenderCommand> m_AlphaRenderCommands;
        std::vector<RenderCommand> m_PostProcessingRenderCommands;
        std::map<RenderTarget*, std::vector<RenderCommand>> m_CustomRenderCommands;


    public:
        CommandBuffer(Renderer* renderer);
        ~CommandBuffer(); 
            
        // pushes render state relevant to a single render call to the command buffer.
        void Push(Mesh* mesh, Material* material, math::mat4 transform = math::mat4(), math::mat4 prevTransform = math::mat4(), math::vec3 boxMin = math::vec3(-99999.0f), math::vec3 boxMax = math::vec3(99999.0f), RenderTarget* target = nullptr);

        // clears the command buffer; usually done after issuing all the stored render commands.
        void Clear();
        // sorts the command buffer; first by shader, then by texture bind.
        // TODO: build an approach using texture arrays (every push would add relevant material textures
        // to texture array (if it wans't there already), and then add a texture index to each material
        // slot; profile if the added texture adjustments actually saves performance!
        void Sort();

        // returns the list of render commands. For minimizing state changes it is advised to first 
        // call Sort() before retrieving and issuing the render commands.
        std::vector<RenderCommand> GetDeferredRenderCommands(bool cull = false);

        // returns the list of render commands of both deferred and forward pushes that require 
        // alpha blending; which have to be rendered last. 
        std::vector<RenderCommand> GetAlphaRenderCommands(bool cull = false);

        // returns the list of custom render commands per render target.
        std::vector<RenderCommand> GetCustomRenderCommands(RenderTarget *target, bool cull = false);

        // returns the list of post-processing render commands.
        std::vector<RenderCommand> GetPostProcessingRenderCommands();

        // returns the list of all render commands with mesh shadow casting
        std::vector<RenderCommand> GetShadowCastRenderCommands();
    };
}

#endif