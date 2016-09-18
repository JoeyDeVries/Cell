#include "command_buffer.h"

#include <algorithm>

#include "../shading/material.h"
#include "../mesh/mesh.h"

namespace Cell
{
    CommandBuffer::CommandBuffer()
    {

    }

    CommandBuffer::~CommandBuffer()
    {
        Clear();
    }

    void CommandBuffer::Push(Mesh *mesh, Material *material, math::mat4 transform)
    {
        RenderCommand command = {};
        command.Mesh      = mesh;
        command.Material  = material;
        command.Transform = transform;
        m_RenderCommands.push_back(command);
    }

    void CommandBuffer::Clear()
    {
        m_RenderCommands.clear();
    }

    // NOTE(Joey): custom per-element sort compare function used by the 
    // CommandBuffer::Sort() function.
    bool customRenderSort(RenderCommand &a, RenderCommand &b)
    {
        //return a.Material->
        return true;
    }
    void CommandBuffer::Sort()
    {
        //std::sort(m_RenderCommands.begin(), m_RenderCommands.end(), customRenderSort);
    }

    std::vector<RenderCommand> CommandBuffer::GetRenderCommands()
    {
        return m_RenderCommands;
    }
};