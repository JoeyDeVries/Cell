#ifndef CELL_POST_PROCESSOR_H
#define CELL_POST_PROCESSOR_H

#include <vector>

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/matrix.h>

namespace Cell
{
    class Texture;
    class RenderTarget;
    class Shader;
    class Renderer;
    class Camera;

    class PostProcessor
    {
    public:
        // resulting post-processor intermediate outputs
        Texture* DownSampledHalfOutput;
        Texture* DownSampledQuarterOutput;
        Texture* DownSampledEightOutput;
        Texture* DownSampledSixteenthOutput;
        Texture* BlurredEightOutput;
        Texture* BlurredSixteenthOutput;
        Texture* SSAOOutput;
        Texture* BloomOutput1;
        Texture* BloomOutput2;
        Texture* BloomOutput3;
        Texture* BloomOutput4;
        Texture* SSROutput;

        // toggles
        bool Sepia    = false;
        bool Vignette = true;
        bool Bloom    = false;
        bool SSAO     = true;
        bool TXAA     = true;
        bool SSR      = false;
        bool MotionBlur      = true;

        // ssao
        int SSAOKernelSize = 32;      
    private:
        // global post-process state
        Shader*       m_PostProcessShader;
        RenderTarget* m_RTOutput;

        // ssao
        RenderTarget* m_SSAORenderTarget;
        Shader* m_SSAOShader;
        Shader* m_SSAOBlurShader;
        Texture* m_SSAONoise;
        // bloom
        RenderTarget* m_BloomRenderTarget0;
        RenderTarget* m_BloomRenderTarget1;
        RenderTarget* m_BloomRenderTarget2;
        RenderTarget* m_BloomRenderTarget3;
        RenderTarget* m_BloomRenderTarget4;
        Shader* m_BloomShader;
        Shader* m_BloomBlurShader;
        // downsample
        RenderTarget* m_DownSampleRTHalf;
        RenderTarget* m_DownSampleRTQuarter;
        RenderTarget* m_DownSampleRTEight;
        RenderTarget* m_DownSampleRTSixteenth;
        Shader* m_DownSampleShader;
        // (lower resolution) downsampled blurs
        RenderTarget* m_DownSampleBlurRTEight;
        RenderTarget* m_DownSampleBlurRTSixteenth;
        // blur
        RenderTarget* m_GaussianRTHalf_H; // to help with ping-ponging (note: do we want this to be a floating point framebuffer as well?)
        RenderTarget* m_GaussianRTQuarter_H;
        RenderTarget* m_GaussianRTEight_H;
        RenderTarget* m_GaussianRTSixteenth_H;
        Shader* m_OnePassGaussianShader;
        // ssr
        RenderTarget *m_SSRRT;
        Shader* m_SSRShader;

    public:
        PostProcessor(Renderer* renderer);
        ~PostProcessor();

        // updates all render targets to match new render size
        void UpdateRenderSize(unsigned int width, unsigned int height);

        // process stages
        void ProcessPreLighting(Renderer* renderer, RenderTarget* gBuffer, Camera *camera);
        void ProcessPostLighting(Renderer* renderer, RenderTarget* gBuffer, RenderTarget* output, Camera *camera);

        // blit all combined post-processing steps to default framebuffer
        void Blit(Renderer* renderer, Texture* soruce);

    private:
        Texture* downsample(Renderer* renderer, Texture* src, RenderTarget* dst);
        Texture* blur(Renderer* renderer, Texture* src,  RenderTarget* dst, int count);
    };
}
#endif