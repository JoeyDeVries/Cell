#include "background.h"

#include "../renderer/renderer.h"
#include "../resources/resources.h"
#include "../shading/material.h"
#include "../shading/shader.h"
#include "../shading/texture_cube.h"
#include "../mesh/cube.h"

namespace Cell
{
    // ------------------------------------------------------------------------
    Background::Background()
    {
        m_Material         = new Material;
        m_Shader           = Resources::LoadShader("background", "shaders/background.vs", "shaders/background.fs");
        m_Material->SetShader(m_Shader);
        m_CubeMap          = Resources::LoadTextureCube("yokohama", "textures/backgrounds/yokohama/");
        m_Mesh             = new Cell::Cube;

        // default material configuration
        m_Material->SetTextureCube("background", m_CubeMap, 0);

        m_Material->DepthCompare = GL_LEQUAL;
    }
    // ------------------------------------------------------------------------
    Background::~Background()
    {
        delete m_Material;
        delete m_Mesh;
    }
    // ------------------------------------------------------------------------
    void Background::SetCubemap(TextureCube *cubemap)
    {
        m_CubeMap = cubemap;
        m_Material->SetTextureCube("background", m_CubeMap, 0);
    }
    // ------------------------------------------------------------------------
    void Background::Render(Renderer *renderer)
    {
        renderer->PushRender(m_Mesh, m_Material, math::mat4());
    }
};