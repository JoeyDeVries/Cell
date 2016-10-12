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
    // NOTE(Joey): custom per-element sort compare function used by the 
    // CommandBuffer::Sort() function.
    bool renderSortDeferred(RenderCommand &a, RenderCommand &b)
    {
        return true;
    }
    bool renderSortCustom(RenderCommand &a, RenderCommand &b)
    {
        return true;
    }
    bool renderSortPostProcessing(RenderCommand &a, RenderCommand &b)
    {
        //return a.Material->
        return true;
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Sort()
    {
        //std::sort(m_RenderCommands.begin(), m_RenderCommands.end(), renderSortDeferred);
        //std::sort(m_RenderCommands.begin(), m_RenderCommands.end(), renderSortCustom);
        //std::sort(m_RenderCommands.begin(), m_RenderCommands.end(), renderSortPostProcessing);
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
    std::vector<RenderCommand> CommandBuffer::GetPostProcessingRenderCommands()
    {
        return m_PostProcessingRenderCommands;
    }
};