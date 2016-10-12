#ifndef CELL_RENDERER_COMMAND_BUFFER_H
#define CELL_RENDERER_COMMAND_BUFFER_H

#include "render_command.h"

#include <math/linear_algebra/matrix.h>

#include <map>
#include <vector>

namespace Cell
{
    class Mesh;
    class Material;
    class RenderTarget;

    /* NOTE(Joey):

      Command buffer.

      Can all be done in a renderer, but merely added via
      composition to keep things organized.

    */
    class CommandBuffer
    {
    public:

    private:
        std::vector<RenderCommand> m_DeferredRenderCommands;
        std::vector<RenderCommand> m_PostProcessingRenderCommands;
        std::map<RenderTarget*, std::vector<RenderCommand>> m_CustomRenderCommands;

    public:
        CommandBuffer();
        ~CommandBuffer(); 
        
        // NOTE(Joey): pushes render state relevant to a single render call
        // to the command buffer.
        void Push(Mesh *mesh, Material *material, math::mat4 transform, RenderTarget *target = nullptr);

        // NOTE(Joey): clears the command buffer; usually done after issuing
        // all the stored render commands.
        void Clear();
        // NOTE(Joey): sorts the command buffer; first by shader, then by 
        // texture bind.
        void Sort();

        // NOTE(Joey): returns the list of render commands. For minimizing
        // state changes it is advised to first call Sort() before retrieving
        // and issuing the render commands.
        std::vector<RenderCommand> GetDeferredRenderCommands();

        // NOTE(Joey): returns the list of custom render commands per render
        // target.
        std::vector<RenderCommand> GetCustomRenderCommands(RenderTarget *target);

        // NOTE(Joey): returns the list of post-processing render commands.
        std::vector<RenderCommand> GetPostProcessingRenderCommands();
    };
}

#endif