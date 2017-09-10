#ifndef CELL_POST_PROCESSOR_H
#define CELL_POST_PROCESSOR_H

namespace Cell
{
    class Texture;
    class RenderTarget;
    class Shader;
    class Renderer;

    class PostProcessor
    {
    public:
        // resulting post-processor intermediate outputs
        Texture* SSAOOutput;
        Texture* BloomOutput;

        // settings
        bool SSAO     = true;
        bool Bloom    = true;
        bool Vignette = true;
        bool TXAA     = true;
        bool SSR      = true;
    private:
        // global post-process state
        Shader*       m_PostProcessShader;
        RenderTarget* m_RTOutput;

        // ssao
        Shader* m_SSAOShader;
        Shader* m_SSAOBlur;
        // bloom
        Shader* m_BloomBlur;
        // blur
        Shader* m_OnePassGaussianShader;

    public:
        PostProcessor();
        ~PostProcessor();

        // process stages
        void ProcessPreLighting(Renderer* renderer, RenderTarget* gBuffer);
        void ProcessPostLighting(Renderer* renderer, RenderTarget* renderOutput);

        // blit all combined post-processing steps to default framebuffer
        void Blit(Renderer* renderer, Texture* soruce);

    private:
        //void blur();
    };
}
#endif