#include "background.h"

#include "scene.h"

#include "../renderer/renderer.h"
#include "../resources/resources.h"
#include "../shading/material.h"
#include "../shading/shader.h"
#include "../shading/texture_cube.h"
#include "../mesh/cube.h"

namespace Cell
{
    // ------------------------------------------------------------------------
    Background::Background() : SceneNode(Scene::CounterID++)
    {
        m_Shader  = Resources::LoadShader("background", "shaders/background.vs", "shaders/background.fs");
        //m_CubeMap = Resources::LoadTextureCube("yokohama", "textures/backgrounds/yokohama/");
        Material  = new Cell::Material(m_Shader);
        Mesh      = new Cell::Cube;

        // default material configuration
        //Material->SetShader(m_Shader);
        //Material->SetTextureCube("background", m_CubeMap, 0);

        Material->DepthCompare = GL_LEQUAL;
    }
    // ------------------------------------------------------------------------
    Background::~Background()
    {
        delete Material;
        delete Mesh;
    }
    // ------------------------------------------------------------------------
    void Background::SetCubemap(TextureCube *cubemap)
    {
        m_CubeMap = cubemap;
        Material->SetTextureCube("background", m_CubeMap, 0);
    }
};