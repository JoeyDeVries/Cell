#include "PBR.h"

#include "renderer.h"
#include "render_target.h"
#include "pbr_capture.h"

#include "../resources/resources.h"
#include "../mesh/cube.h"
#include "../mesh/sphere.h"
#include "../scene/scene.h"
#include "../scene/scene_node.h"
#include "../shading/material.h"
#include "../shading/shader.h"
#include "../shading/texture_cube.h"
#include "../camera/camera.h"

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
        m_SceneEnvCube = new SceneNode(0);
        m_SceneEnvCube->Mesh = m_PBRCaptureCube;
        m_SceneEnvCube->Material = m_PBRHdrToCubemap;

        // - brdf integration
        m_Renderer->Blit(nullptr, m_RenderTargetBRDFLUT, m_PBRIntegrateBRDF);

        // capture
        m_ProbeCaptureShader = Resources::LoadShader("pbr:capture", "shaders/pbr/capture.vs", "shaders/pbr/capture.fs");
        m_ProbeCaptureShader->Use();
        m_ProbeCaptureShader->SetInt("TexAlbedo", 0);
        m_ProbeCapture = new Material(m_ProbeCaptureShader);

        // debug render
        m_ProbeDebugSphere = new Sphere(32, 32);
        m_ProbeDebugShader = Cell::Resources::LoadShader("pbr:probe_render", "shaders/pbr/probe_render.vs", "shaders/pbr/probe_render.fs");
        m_ProbeDebugShader->Use();
        m_ProbeDebugShader->SetInt("PrefilterMap", 0);
    }
    // --------------------------------------------------------------------------------------------
    PBR::~PBR()
    {
        delete m_PBRCaptureCube;
        delete m_SceneEnvCube;
        delete m_RenderTargetBRDFLUT;
        delete m_PBRHdrToCubemap;
        delete m_PBRIrradianceCapture;
        delete m_PBRPrefilterCapture;
        delete m_PBRIntegrateBRDF;
        delete m_SkyCapture;
        for (unsigned int i = 0; i < m_CaptureProbes.size(); ++i)
        {
            delete m_CaptureProbes[i]->Irradiance;
            delete m_CaptureProbes[i]->Prefiltered;
            delete m_CaptureProbes[i];
        }
        delete m_ProbeCapture;
        delete m_ProbeDebugSphere;
    }
    // --------------------------------------------------------------------------------------------
    void PBR::SetSkyCapture(PBRCapture* capture)
    {
        m_SkyCapture = capture;
    }

    // --------------------------------------------------------------------------------------------
    void PBR::AddCapture(PBRCapture* capture, math::vec3 position)
    {
        capture->Position = position;
        m_CaptureProbes.push_back(capture);
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
        m_Renderer->renderToCubemap(m_SceneEnvCube, &hdrEnvMap);

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
        m_Renderer->renderToCubemap(m_SceneEnvCube, captureProbe->Irradiance, math::vec3(0.0f), 0);
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
            m_Renderer->renderToCubemap(m_SceneEnvCube, captureProbe->Prefiltered, math::vec3(0.0f), i);

        }
        //m_CaptureProbes.push_back(captureProbe);
        return captureProbe;
    }
    // --------------------------------------------------------------------------------------------
    PBRCapture* PBR::GetSkyCapture()
    {
        return m_SkyCapture;
    }
    // --------------------------------------------------------------------------------------------
    PBRCapture* PBR::GetClosestCapture(math::vec3 position)
    {
        PBRCapture* closestCapture = nullptr;
        float closestLength = 100000.0;
        for (int i = 0; i < m_CaptureProbes.size(); ++i)
        {
            float lengthSq = math::lengthSquared((m_CaptureProbes[i]->Position - position));
            if (lengthSq < closestLength)
            {
                closestCapture = m_CaptureProbes[i];
                closestLength = lengthSq;
            }
        }
        // fall back to sky capture if no reflection probe was found
        if (!closestCapture)
        {
            closestCapture = m_SkyCapture;
        }
        return closestCapture;
    }
    // --------------------------------------------------------------------------------------------
    void PBR::RenderCaptures()
    {
        m_ProbeDebugShader->Use();
        m_ProbeDebugShader->SetMatrix("projection", m_Renderer->GetCamera()->Projection);
        m_ProbeDebugShader->SetMatrix("view", m_Renderer->GetCamera()->View);
        m_ProbeDebugShader->SetVector("CamPos", m_Renderer->GetCamera()->Position);

        // first do the sky capture (at position (0, 0, 0)?)
        m_ProbeDebugShader->SetVector("Position", math::vec3(0.0f));
        m_SkyCapture->Prefiltered->Bind(0);
        m_Renderer->renderMesh(m_ProbeDebugSphere, m_ProbeDebugShader);

        // then do the same for each capture probe (at their respective location)
        for (int i = 0; i < m_CaptureProbes.size(); ++i)
        {
            m_ProbeDebugShader->SetVector("Position", m_CaptureProbes[i]->Position);
            m_CaptureProbes[i]->Prefiltered->Bind(0);
            m_Renderer->renderMesh(m_ProbeDebugSphere, m_ProbeDebugShader);
        }
    }
}
