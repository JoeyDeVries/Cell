#include "pbr_test.h"

#include <cell/shading/material.h>
#include <cell/resources/resources.h>
#include <cell/scene/scene.h>
#include <cell/scene/scene_node.h>
#include <cell/camera/camera.h>

ScenePbrTest::ScenePbrTest(Cell::Renderer *renderer, Cell::Camera *camera)
{
     m_Renderer = renderer;
     m_Camera = camera;

     m_PbrShader = Cell::Resources::LoadShader("pbr", "shaders/pbr.vs", "shaders/pbr.frag");
     m_PBRMaterial.SetShader(m_PbrShader);
}

ScenePbrTest::~ScenePbrTest()
{

}

void ScenePbrTest::Init()
{
    // TODO(Joey): pre-compute irradiance/prefilter/integrate map  for testing
}

void ScenePbrTest::Update(float dt)
{
    m_Camera->Update(dt);
}

void ScenePbrTest::Render()
{

}