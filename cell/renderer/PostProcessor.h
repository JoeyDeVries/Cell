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

    /*

      Manages and maintains all data and functionality related to end-of-frame post-processing. 
      This doesn't only include post-processing effects like SSAO, Bloom, Vignette, but also 
      includes general functionality like blitting, blurring, HDR and gammac-orrection. 
      The post-processor's multi-pass effects support multiple resolutions and adjusts accordingly 
      when a resolution change occurs.

    */
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

        // toggles
        bool Sepia      = false;
        bool Vignette   = true;
        bool Bloom      = true;
        bool SSAO       = true;
        bool TXAA       = false;
        bool MotionBlur = true;

        // ssao
        int SSAOKernelSize = 32;      

        // motion-blur
        float FPSTarget = 60.0;
    private:
        // global post-process state
        Shader*       m_PostProcessShader;
        RenderTarget* m_RTOutput;

        // ssao
        RenderTarget* m_SSAORenderTarget;
        Shader*       m_SSAOShader;
        Shader*       m_SSAOBlurShader;
        Texture*      m_SSAONoise;
        // bloom
        RenderTarget* m_BloomRenderTarget0;
        RenderTarget* m_BloomRenderTarget1;
        RenderTarget* m_BloomRenderTarget2;
        RenderTarget* m_BloomRenderTarget3;
        RenderTarget* m_BloomRenderTarget4;
        Shader*       m_BloomShader;
        Shader*       m_BloomBlurShader;
        // downsample
        RenderTarget* m_DownSampleRTHalf;
        RenderTarget* m_DownSampleRTQuarter;
        RenderTarget* m_DownSampleRTEight;
        RenderTarget* m_DownSampleRTSixteenth;
        Shader*       m_DownSampleShader;
        // (lower resolution) downsampled blurs
        RenderTarget* m_DownSampleBlurRTEight;
        RenderTarget* m_DownSampleBlurRTSixteenth;
        // blur
        RenderTarget* m_GaussianRTHalf_H; 
        RenderTarget* m_GaussianRTQuarter_H;
        RenderTarget* m_GaussianRTEight_H;
        RenderTarget* m_GaussianRTSixteenth_H;
        Shader*       m_OnePassGaussianShader;
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