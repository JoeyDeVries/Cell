#include "background.h"

#include "../resources/resources.h"
#include "../shading/material.h"
#include "../shading/shader.h"
#include "../shading/texture_cube.h"
//#include "../mesh/cube.h"

namespace Cell
{
    Background::Background()
    {
        //Resources::LoadShader("background", "shaders/background.vs", "shaders/background.fs");
        //m_Material.Shader
        
    }

    Background::~Background()
    {

    }

    void Background::SetCubemap(TextureCube cubemap)
    {

    }

    void Background::Render()
    {

    }
};