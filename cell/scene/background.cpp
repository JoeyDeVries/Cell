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
    // --------------------------------------------------------------------------------------------
    Background::Background() : SceneNode(Scene::CounterID++)
    {
        Scene::Root->AddChild(this);

        m_Shader  = Resources::LoadShader("background", "shaders/background.vs", "shaders/background.fs");
        Material  = new Cell::Material(m_Shader);
        Mesh      = new Cell::Cube;
        BoxMin    = math::vec3(-99999.0);
        BoxMax    = math::vec3( 99999.0);

        // default material configuration
        Material->SetFloat("Exposure", 1.0f);
        Material->DepthCompare = GL_LEQUAL;
        Material->Cull = false;
        Material->ShadowCast = false;
        Material->ShadowReceive = false;
    }
    // --------------------------------------------------------------------------------------------
    Background::~Background()
    {

    }
    // --------------------------------------------------------------------------------------------
    void Background::SetCubemap(TextureCube* cubemap)
    {
        m_CubeMap = cubemap;
        Material->SetTextureCube("background", m_CubeMap, 0);
    }
};