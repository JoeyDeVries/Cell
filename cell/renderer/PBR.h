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

    class PBR
    {
        friend Renderer;
    private:
        Renderer* m_Renderer;

        std::vector<PBRCapture*> m_CaptureProbes;
        PBRCapture*              m_SkyCapture;
        RenderTarget*            m_RenderTargetBRDFLUT;

        Material* m_PBRHdrToCubemap;
        Material* m_PBRIrradianceCapture;
        Material* m_PBRPrefilterCapture;
        Material* m_PBRIntegrateBRDF;

        Mesh*      m_PBRCaptureCube;
        SceneNode* m_SceneEnvCube;

    public:
        PBR(Renderer* renderer);
        ~PBR();

        // asd
        PBRCapture* ProcessEquirectangular(Texture* envMap);
        // asd
        PBRCapture* ProcessCube(TextureCube *capture, math::vec3 position, float radius);

        PBRCapture* GetClosestCapture(math::vec3 position);
    };
}

#endif