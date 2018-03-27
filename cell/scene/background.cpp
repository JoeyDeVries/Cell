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
        material  = new Cell::Material(m_Shader);
        mesh      = new Cell::Cube;
        boxMin    = math::vec3(-99999.0);
        boxMax    = math::vec3( 99999.0);

        // default material configuration
        material->SetFloat("Exposure", 1.0f);
        material->DepthCompare = GL_LEQUAL;
        material->Cull = false;
        material->ShadowCast = false;
        material->ShadowReceive = false;
    }
    // --------------------------------------------------------------------------------------------
    Background::~Background()
    {

    }
    // --------------------------------------------------------------------------------------------
    void Background::SetCubemap(TextureCube* cubemap)
    {
        m_CubeMap = cubemap;
        material->SetTextureCube("background", m_CubeMap, 0);
    }
};