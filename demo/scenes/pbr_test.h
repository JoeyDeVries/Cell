#ifndef SCENE_PBR_TEST_H
#define SCENE_PBR_TEST_H

#include <cell/renderer/renderer.h>
#include <cell/shading/material.h>
#include <cell/shading/shader.h>

class ScenePbrTest 
{
private:
    Cell::Renderer *m_Renderer;
    Cell::Camera   *m_Camera;

    Cell::Material m_PBRMaterial;
    Cell::Material m_PBRIrradianceMaterial;
    Cell::Material m_PBRPrefilterMaterial;
    Cell::Material m_PBRBRDFIntegrateMaterial;
    Cell::Shader   *m_PbrShader;
    Cell::Shader   *m_PbrIrradianceShader;
    Cell::Shader   *m_PbrPrefilterShader;
    Cell::Shader   *m_PbrBRDFintegrateShader;

public:
    // NOTE(Joey): pass over relevant data per scene: dependancy injection
    ScenePbrTest(Cell::Renderer *renderer, Cell::Camera *camera);
    ~ScenePbrTest();

    void Init();

    void Update(float dt);
    void Render();
};

#endif