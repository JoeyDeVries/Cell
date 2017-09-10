#include "MaterialLibrary.h"

#include "render_target.h"

#include "../shading/material.h"
#include "../resources/resources.h"

#include <utility/logging/log.h>
#include <utility/string_id.h>

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    MaterialLibrary::MaterialLibrary(RenderTarget *gBuffer)
    {
        generateDefaultMaterials();
        generateInternalMaterials(gBuffer);
    }
    // --------------------------------------------------------------------------------------------
    MaterialLibrary::~MaterialLibrary()
    {
        for (auto it = m_DefaultMaterials.begin(); it != m_DefaultMaterials.end(); ++it)
        {
            delete it->second;
        }
        for (unsigned int i = 0; i < m_Materials.size(); ++i)
        {
            delete m_Materials[i];
        }
        delete debugLightMaterial;

        delete defaultBlitMaterial;
        delete postProcessingMaterial;

        delete PBRHdrToCubemap;
        delete PBRIrradianceCapture;
        delete PBRPrefilterCapture;
        delete PBRIntegrateBRDF;
    }
    // --------------------------------------------------------------------------------------------
    Material* MaterialLibrary::CreateMaterial(std::string base)
    {
        auto found = m_DefaultMaterials.find(SID(base));
        if (found != m_DefaultMaterials.end())
        {
            Material copy = found->second->Copy();
            Material *mat = new Material(copy);
            m_Materials.push_back(mat); // TODO(Joey): a bit ugly for now, come up with more proper memory management scheme for materials
            return mat;
        }
        else
        {
            Log::Message("Material of template: " + base + " requested, but template did not exist.", LOG_ERROR);
            return nullptr;
        }
    }
    // --------------------------------------------------------------------------------------------
    Material* MaterialLibrary::CreateCustomMaterial(Shader *shader)
    {
        Material *mat = new Material(shader);
        mat->Type = MATERIAL_CUSTOM;
        m_Materials.push_back(mat);
        return mat;
    }
    // --------------------------------------------------------------------------------------------
    Material* MaterialLibrary::CreatePostProcessingMaterial(Shader *shader)
    {
        Material *mat = new Material(shader);
        mat->Type = MATERIAL_POST_PROCESS;
        m_Materials.push_back(mat);
        return mat;
    }
    // --------------------------------------------------------------------------------------------    
    void MaterialLibrary::generateDefaultMaterials()
    {
        // default render material (deferred path)
        Shader *defaultShader = Resources::LoadShader("default", "shaders/deferred/g_buffer.vs", "shaders/deferred/g_buffer.fs");
        Material *defaultMat = new Material(defaultShader);
        defaultMat->Type = MATERIAL_DEFAULT;
        defaultMat->SetTexture("TexAlbedo", Resources::LoadTexture("default albedo", "textures/checkerboard.png", GL_TEXTURE_2D, GL_RGB), 3);
        defaultMat->SetTexture("TexNormal", Resources::LoadTexture("default normal", "textures/norm.png"), 4);
        defaultMat->SetTexture("TexMetallic", Resources::LoadTexture("default metallic", "textures/black.png"), 5);
        defaultMat->SetTexture("TexRoughness", Resources::LoadTexture("default roughness", "textures/checkerboard.png"), 6);
        defaultMat->SetTexture("TexAO", Resources::LoadTexture("default ao", "textures/white.png"), 7);
        m_DefaultMaterials[SID("default")] = defaultMat;
        // glass material
        Shader *glassShader = Resources::LoadShader("glass", "shaders/pbr/pbr.vs", "shaders/pbr/pbr.fs", { "ALPHA" });
        Material *glassMat = new Material(glassShader);
        glassMat->Type = MATERIAL_CUSTOM; // this material can't fit in the deferred rendering pipeline (due to transparency sorting).
        glassMat->SetTexture("TexAlbedo", Cell::Resources::LoadTexture("glass albedo", "textures/glass.png", GL_TEXTURE_2D, GL_RGB), 3);
        glassMat->SetTexture("TexNormal", Cell::Resources::LoadTexture("glass normal", "textures/pbr/plastic/normal.png"), 4);
        glassMat->SetTexture("TexMetallic", Cell::Resources::LoadTexture("glass metallic", "textures/pbr/plastic/metallic.png"), 5);
        glassMat->SetTexture("TexRoughness", Cell::Resources::LoadTexture("glass roughness", "textures/pbr/plastic/roughness.png"), 6);
        glassMat->SetTexture("TexAO", Cell::Resources::LoadTexture("glass ao", "textures/pbr/plastic/ao.png"), 7);
        glassMat->Blend = true;
        m_DefaultMaterials[SID("glass")] = glassMat;
        // alpha blend material

        // alpha cutout material
    }
    // --------------------------------------------------------------------------------------------
    void MaterialLibrary::generateInternalMaterials(RenderTarget *gBuffer)
    {
        // post-processing
        Shader* defaultBlitShader = Cell::Resources::LoadShader("blit", "shaders/screen_quad.vs", "shaders/default_blit.fs");
        defaultBlitMaterial = new Material(defaultBlitShader);
        Shader* postProcessingShader = Cell::Resources::LoadShader("post processing", "shaders/screen_quad.vs", "shaders/post_processing.fs");
        postProcessingMaterial = new Material(postProcessingShader);

        // deferred
        deferredAmbientShader     = Cell::Resources::LoadShader("deferred ambient", "shaders/screen_quad.vs", "shaders/deferred/ambient.fs");
        deferredDirectionalShader = Cell::Resources::LoadShader("deferred directional", "shaders/screen_quad.vs", "shaders/deferred/directional.fs");
        deferredPointShader       = Cell::Resources::LoadShader("deferred point", "shaders/deferred/point.vs", "shaders/deferred/point.fs");

        deferredAmbientShader->Use();
        deferredAmbientShader->SetInt("gPositionMetallic", 0);
        deferredAmbientShader->SetInt("gNormalRoughness", 1);
        deferredAmbientShader->SetInt("gAlbedoAO", 2);
        deferredAmbientShader->SetInt("envIrradiance", 3);
        deferredAmbientShader->SetInt("envPrefilter", 4);
        deferredAmbientShader->SetInt("BRDFLUT", 5);
        deferredAmbientShader->SetInt("SSAO", 6);
        deferredDirectionalShader->Use();
        deferredDirectionalShader->SetInt("gPositionMetallic", 0);
        deferredDirectionalShader->SetInt("gNormalRoughness", 1);
        deferredDirectionalShader->SetInt("gAlbedoAO", 2);
        deferredPointShader->Use();
        deferredPointShader->SetInt("gPositionMetallic", 0);
        deferredPointShader->SetInt("gNormalRoughness", 1);
        deferredPointShader->SetInt("gAlbedoAO", 2);

        // debug
        Shader *debugLightShader = Resources::LoadShader("debug light", "shaders/light.vs", "shaders/light.fs");
        debugLightMaterial = new Material(debugLightShader);
    }
}