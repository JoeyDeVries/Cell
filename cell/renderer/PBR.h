#ifndef CELL_PBR_H
#define CELL_PBR_H

#include <vector>

#include <math/linear_algebra/vector.h>

namespace Cell
{
    class Renderer;
    class Material;
    class Mesh;
    class RenderTarget;
    class Texture;
    class TextureCube;
    class PBRCapture;
    class SceneNode;
    class Shader;

    class PBR
    {
        friend Renderer;
    private:
        Renderer* m_Renderer;

        std::vector<PBRCapture*> m_CaptureProbes;
        PBRCapture*              m_SkyCapture;
        RenderTarget*            m_RenderTargetBRDFLUT;

        // pbr pre-processing (irradiance/pre-filter)
        Material* m_PBRHdrToCubemap;
        Material* m_PBRIrradianceCapture;
        Material* m_PBRPrefilterCapture;
        Material* m_PBRIntegrateBRDF;
        Mesh*      m_PBRCaptureCube;
        SceneNode* m_SceneEnvCube;

        // reflection probe capture
        Shader*   m_ProbeCaptureShader;
        Material* m_ProbeCapture;

        // debug 
        Mesh*   m_ProbeDebugSphere;
        Shader* m_ProbeDebugShader;

    public:
        PBR(Renderer* renderer);
        ~PBR();

        // asd
        void SetSkyCapture(PBRCapture* capture);
        // asd
        void AddCapture(PBRCapture* capture, math::vec3 position);
        // asd
        PBRCapture* ProcessEquirectangular(Texture* envMap);
        // asd
        PBRCapture* ProcessCube(TextureCube *capture, math::vec3 position, float radius);


        PBRCapture* GetSkyCapture();
        PBRCapture* GetClosestCapture(math::vec3 position);

        void RenderCaptures();
    };
}

#endif