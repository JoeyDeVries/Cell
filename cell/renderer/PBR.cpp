#include "PBR.h"

#include "render_target.h"
#include "pbr_capture.h"

#include "../resources/resources.h"
#include "../mesh/cube.h"
#include "../scene/scene.h"
#include "../scene/scene_node.h"
#include "../shading/material.h"
#include "../shading/texture_cube.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    PBR::PBR(Renderer* renderer)
    {
        m_Renderer = renderer;
        
        m_RenderTargetBRDFLUT = new RenderTarget(128, 128, GL_HALF_FLOAT, 1, true);
        Cell::Shader *hdrToCubemap      = Cell::Resources::LoadShader("pbr:hdr_to_cubemap", "shaders/pbr/cube_sample.vs", "shaders/pbr/spherical_to_cube.fs");
        Cell::Shader *irradianceCapture = Cell::Resources::LoadShader("pbr:irradiance", "shaders/pbr/cube_sample.vs", "shaders/pbr/irradiance_capture.fs");
        Cell::Shader *prefilterCapture  = Cell::Resources::LoadShader("pbr:prefilter", "shaders/pbr/cube_sample.vs", "shaders/pbr/prefilter_capture.fs");
        Cell::Shader *integrateBrdf     = Cell::Resources::LoadShader("pbr:integrate_brdf", "shaders/screen_quad.vs", "shaders/pbr/integrate_brdf.fs");
        m_PBRHdrToCubemap      = new Material(hdrToCubemap);
        m_PBRIrradianceCapture = new Material(irradianceCapture);
        m_PBRPrefilterCapture  = new Material(prefilterCapture);
        m_PBRIntegrateBRDF     = new Material(integrateBrdf);
        m_PBRHdrToCubemap->DepthCompare      = GL_LEQUAL;
        m_PBRIrradianceCapture->DepthCompare = GL_LEQUAL;
        m_PBRPrefilterCapture->DepthCompare  = GL_LEQUAL;

        m_PBRCaptureCube = new Cube();
        m_SceneEnvCube   = Cell::Scene::MakeSceneNode(m_PBRCaptureCube, m_PBRHdrToCubemap);

        // - brdf integration
        m_Renderer->Blit(nullptr, m_RenderTargetBRDFLUT, m_PBRIntegrateBRDF);
    }
    // --------------------------------------------------------------------------------------------
    PBR::~PBR()
    {
        delete m_PBRCaptureCube;
        delete m_RenderTargetBRDFLUT;
        delete m_PBRHdrToCubemap;
        delete m_PBRIrradianceCapture;
        delete m_PBRPrefilterCapture;
        delete m_PBRIntegrateBRDF;
        for (unsigned int i = 0; i < m_CaptureProbes.size(); ++i)
        {
            delete m_CaptureProbes[i]->Irradiance;
            delete m_CaptureProbes[i]->Prefiltered;
            delete m_CaptureProbes[i];
        }
    }
    // --------------------------------------------------------------------------------------------
    PBRCapture* PBR::ProcessEquirectangular(Texture* envMap)
    {
        // convert HDR radiance image to HDR environment cubemap
        m_SceneEnvCube->Material = m_PBRHdrToCubemap;
        m_PBRHdrToCubemap->SetTexture("environment", envMap, 0);
        // TODO(Joey): think of proper way to manage memory here.
        Cell::TextureCube hdrEnvMap;
        hdrEnvMap.DefaultInitialize(128, 128, GL_RGB, GL_FLOAT);
        m_Renderer->RenderToCubemap(m_SceneEnvCube, &hdrEnvMap);

        return ProcessCube(&hdrEnvMap, math::vec3(0.0f), 0.0f);
    }
    // --------------------------------------------------------------------------------------------
    PBRCapture* PBR::ProcessCube(TextureCube* capture, math::vec3 position, float radius)
    {
        PBRCapture* captureProbe = new PBRCapture;

        // irradiance
        captureProbe->Irradiance = new TextureCube;
        captureProbe->Irradiance->DefaultInitialize(32, 32, GL_RGB, GL_FLOAT);
        m_PBRIrradianceCapture->SetTextureCube("environment", capture, 0);
        m_SceneEnvCube->Material = m_PBRIrradianceCapture;
        m_Renderer->RenderToCubemap(m_SceneEnvCube, captureProbe->Irradiance, math::vec3(0.0f), 0);
        // prefilter 
        captureProbe->Prefiltered = new TextureCube;;
        captureProbe->Prefiltered->FilterMin = GL_LINEAR_MIPMAP_LINEAR;
        captureProbe->Prefiltered->DefaultInitialize(128, 128, GL_RGB, GL_FLOAT, true);
        m_PBRPrefilterCapture->SetTextureCube("environment", capture, 0);
        m_SceneEnvCube->Material = m_PBRPrefilterCapture;
        // calculate prefilter for multiple roughness levels
        unsigned int maxMipLevels = 5;
        for (unsigned int i = 0; i < maxMipLevels; ++i)
        {
            m_PBRPrefilterCapture->SetFloat("roughness", (float)i / (float)(maxMipLevels - 1));
            m_Renderer->RenderToCubemap(m_SceneEnvCube, captureProbe->Prefiltered, math::vec3(0.0f), i);

        }
        m_CaptureProbes.push_back(captureProbe);
        return captureProbe;
    }
    // --------------------------------------------------------------------------------------------
    PBRCapture* PBR::GetClosestCapture(math::vec3 position)
    {
        return m_CaptureProbes[0];
    }
}
