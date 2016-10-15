#include "command_buffer.h"

#include <algorithm>

#include "../shading/material.h"
#include "../mesh/mesh.h"

namespace Cell
{
    // ------------------------------------------------------------------------
    CommandBuffer::CommandBuffer()
    {

    }
    // ------------------------------------------------------------------------
    CommandBuffer::~CommandBuffer()
    {
        Clear();
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Push(Mesh *mesh, Material *material, math::mat4 transform, RenderTarget *target)
    {
        RenderCommand command = {};
        command.Mesh      = mesh;
        command.Material  = material;
        command.Transform = transform;

        // NOTE(Joey): check the type of the material and process differently
        // if necessary
        //if (material->Type == MATERIAL_DEFAULT)
        //{
            //m_DeferredRenderCommands.push_back(command);
        //}
        /*else*/ if (material->Type == MATERIAL_CUSTOM || material->Type == MATERIAL_DEFAULT) // NOTE(Joey): we include default here for now as we don't have the deferred pipeline yet
        {
            // NOTE(Joey): check if this render target has been pushed before, if
            // so add to vector, otherwise create new vector with this render
            // target.
            if (m_CustomRenderCommands.find(target) != m_CustomRenderCommands.end())
                m_CustomRenderCommands[target].push_back(command);
            else
            {
                m_CustomRenderCommands[target] = std::vector<RenderCommand>();
                m_CustomRenderCommands[target].push_back(command);
            }
        }
        else if (material->Type == MATERIAL_POST_PROCESS)
        {
            m_PostProcessingRenderCommands.push_back(command);
        }
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Clear()
    {
        m_DeferredRenderCommands.clear();
        m_CustomRenderCommands.clear();
        m_PostProcessingRenderCommands.clear();
    }
    // ------------------------------------------------------------------------
    // TODO(Joey): can we combine these render sorts in one sort? First attempts failed so it's 
    // likely this isn't possible; check w/ community!
    // NOTE(Joey): custom per-element sort compare function used by the 
    // CommandBuffer::Sort() function.
    bool renderSortDeferred(const RenderCommand &a, const RenderCommand &b)
    {
        return false;
    }
    // NOTE(Joey): first sort on alpha state
    bool renderSortAlpha(const RenderCommand &a, const RenderCommand &b)
    {       
        return a.Material->Blend < b.Material->Blend;
    }
    bool renderSortShader(const RenderCommand &a, const RenderCommand &b)
    {
        return a.Material->GetShader()->ID < b.Material->GetShader()->ID;
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Sort()
    {
        std::sort(m_DeferredRenderCommands.begin(), m_DeferredRenderCommands.end(), renderSortDeferred);
        for (auto rtIt = m_CustomRenderCommands.begin(); rtIt != m_CustomRenderCommands.end(); rtIt++) 
        {
            // NOTE(Joey): remember to sort in reverse order of signifiance
            std::sort(rtIt->second.begin(), rtIt->second.end(), renderSortShader);
            std::sort(rtIt->second.begin(), rtIt->second.end(), renderSortAlpha);
        }
    }
    // ------------------------------------------------------------------------
    std::vector<RenderCommand> CommandBuffer::GetDeferredRenderCommands()
    {
        return m_DeferredRenderCommands;
    }
    // ------------------------------------------------------------------------
    std::vector<RenderCommand> CommandBuffer::GetCustomRenderCommands(RenderTarget *target)
    {
        return m_CustomRenderCommands[target];
    }
    // ------------------------------------------------------------------------
    std::vector<RenderCommand> CommandBuffer::GetDeferredAlphaRenderCommands()
    {
        return m_DeferredAlphaRenderCommands;
    }
    // ------------------------------------------------------------------------
    std::vector<RenderCommand> CommandBuffer::GetPostProcessingRenderCommands()
    {
        return m_PostProcessingRenderCommands;
    }
};