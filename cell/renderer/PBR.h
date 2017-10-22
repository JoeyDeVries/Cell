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
    class PostProcessor;

    /*

      Manages and maintains all render data and functionality related to the (main) deferred PBR
      pipeline. This includes (pre)processing (captured) reflection data (solving the render 
      equation integral) for use in later rendering. 

    */
    class PBR
    {
        friend Renderer;
        friend PostProcessor;
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

        // irradiance capture 
        Shader*   m_ProbeCaptureShader;
        Shader*   m_ProbeCaptureBackgroundShader;

        // debug 
        Mesh*   m_ProbeDebugSphere;
        Shader* m_ProbeDebugShader;

    public:
        PBR(Renderer* renderer);
        ~PBR();

        // sets the combined irradiance/pre-filter global environment skylight 
        void SetSkyCapture(PBRCapture* capture);
        // adds a processed PBR capture to the list of irradiance probes
        void AddIrradianceProbe(PBRCapture* capture, math::vec3 position, float radius);
        // removes all irradiance probe entries from the global GI grid
        void ClearIrradianceProbes();
        // generate an irradiance and pre-filter map out of a 2D equirectangular map (preferably HDR)
        PBRCapture* ProcessEquirectangular(Texture* envMap);
        // generate an irradiance and pre-filter map out of a cubemap texture
        PBRCapture* ProcessCube(TextureCube *capture, bool prefilter = true);
        
        // retrieves the environment skylight 
        PBRCapture* GetSkyCapture();
        // retrieve all pushed irradiance probes
        std::vector<PBRCapture*> GetIrradianceProbes(math::vec3 queryPos, float queryRadius);

        // renders all reflection/irradiance probes for visualization/debugging.
        void RenderProbes();
    };
}

#endif