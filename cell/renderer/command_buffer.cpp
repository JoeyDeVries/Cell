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
    void CommandBuffer::Push(RenderTarget *target, Mesh *mesh, Material *material, math::mat4 transform)
    {
        RenderCommand command = {};
        command.Mesh      = mesh;
        command.Material  = material;
        command.Transform = transform;

        // NOTE(Joey): check if this render target has been pushed before, if
        // so add to vector, otherwise create new vector with this render
        // target.
        if(m_RenderCommands.find(target) != m_RenderCommands.end())
            m_RenderCommands[target].push_back(command);
        else
        {
            m_RenderCommands[target] = std::vector<RenderCommand>();
            m_RenderCommands[target].push_back(command);
        }
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Clear()
    {
        m_RenderCommands.clear();
    }
    // ------------------------------------------------------------------------
    // NOTE(Joey): custom per-element sort compare function used by the 
    // CommandBuffer::Sort() function.
    bool customRenderSort(RenderCommand &a, RenderCommand &b)
    {
        //return a.Material->
        return true;
    }
    // ------------------------------------------------------------------------
    void CommandBuffer::Sort()
    {
        //std::sort(m_RenderCommands.begin(), m_RenderCommands.end(), customRenderSort);
    }
    // ------------------------------------------------------------------------
    std::vector<RenderCommand> CommandBuffer::GetRenderCommands(RenderTarget *target)
    {
        return m_RenderCommands[target];
    }
};