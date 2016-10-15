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
        // TODO(Joey): how do we combine alpha object logic w/ custom render targets; should we 
        // even allow this?
        // TODO(Joey): perhaps we want to retain alpha renders as within their custom render 
        // commands but sort them on their alpha state, and only store seperately for deferred
        // render commands?
        std::vector<RenderCommand> m_DeferredAlphaRenderCommands;
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

        // NOTE(Joey): returns the list of render commands of both deferred and forward pushes that
        // require alpha blending; which have to be rendered last. (Should we also sort them here
        // based on camera distance, or keep the order as supplied by the user when pushing the
        // render commands; passing responsibility to the end user, so don't sort at all?).
        // NOTE(Joey): first we try them out on deferred only, we can likely get away with sorting
        // on alpha state (as last) in custom render commands s.t. it will still work properly
        // with each supplied render target.
        std::vector<RenderCommand> GetDeferredAlphaRenderCommands();

        // NOTE(Joey): returns the list of custom render commands per render
        // target.
        std::vector<RenderCommand> GetCustomRenderCommands(RenderTarget *target);

        // NOTE(Joey): returns the list of post-processing render commands.
        std::vector<RenderCommand> GetPostProcessingRenderCommands();
    };
}

#endif