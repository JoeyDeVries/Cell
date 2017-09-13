#include "PostProcessor.h"

#include "render_target.h"
#include "renderer.h"

#include <random>

#include "../resources/resources.h"
#include "../shading/texture.h"
#include "../shading/shader.h"
#include "../camera/camera.h"

#include <math/common.h>

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    PostProcessor::PostProcessor()
    {
        // global post-processing shader
        {
            m_PostProcessShader = Cell::Resources::LoadShader("post process", "shaders/screen_quad.vs", "shaders/post_processing.fs");
            m_PostProcessShader->Use();
            m_PostProcessShader->SetInt("TexSrc", 0);
            m_PostProcessShader->SetInt("TexBloom", 1);
        }

        // gaussian blur shader
        {
            m_GaussianRT512_H = new RenderTarget(512, 512, GL_HALF_FLOAT, 1, false); // TODO: 16-bit float? and resolution? make agnostic
            m_GaussianRT512_V = new RenderTarget(512, 512, GL_HALF_FLOAT, 1, false); // TODO: 16-bit float? and resolution? make agnostic
            m_GaussianRT256_H = new RenderTarget(256, 256, GL_HALF_FLOAT, 1, false); 
            m_GaussianRT256_V = new RenderTarget(256, 256, GL_HALF_FLOAT, 1, false); 
            m_GaussianRT128_H = new RenderTarget(128, 128, GL_HALF_FLOAT, 1, false);
            m_GaussianRT128_V = new RenderTarget(128, 128, GL_HALF_FLOAT, 1, false);
            m_GaussianRT64_H  = new RenderTarget(64, 64, GL_HALF_FLOAT, 1, false);
            m_GaussianRT64_V  = new RenderTarget(64, 64, GL_HALF_FLOAT, 1, false);

            m_OnePassGaussianShader = Cell::Resources::LoadShader("gaussian blur", "shaders/screen_quad.vs", "shaders/post/blur_guassian.fs");
            m_OnePassGaussianShader->Use();
            m_OnePassGaussianShader->SetInt("TexSrc", 0);
        }

        // ssao
        {
            m_SSAORenderTarget = new RenderTarget(1280, 720, GL_HALF_FLOAT, 1, false);
            SSAOOutput = m_SSAORenderTarget->GetColorTexture(0);

            m_SSAOShader = Cell::Resources::LoadShader("ssao", "shaders/screen_quad.vs", "shaders/post/ssao.fs");
            m_SSAOShader->Use();
            m_SSAOShader->SetInt("gPositionMetallic", 0);
            m_SSAOShader->SetInt("gNormalRoughness", 1);
            m_SSAOShader->SetInt("texNoise", 2);

            std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
            std::default_random_engine generator;
            std::vector<math::vec3> ssaoKernel;
            for (int i = 0; i < SSAOKernelSize; ++i)
            {
                math::vec3 sample(
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator) * 2.0f - 1.0f,
                    randomFloats(generator)
                );
                sample = math::normalize(sample);
                sample = sample * randomFloats(generator);
                float scale = (float)i / (float)SSAOKernelSize;
                scale = math::lerp(0.1f, 1.0f, scale * scale);
                sample = sample * scale;
                ssaoKernel.push_back(sample);
            }
            std::vector<math::vec3> ssaoNoise;
            for (unsigned int i = 0; i < 16; i++)
            {
                math::vec3 noise(
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    0.0f);
                ssaoNoise.push_back(noise);
            }
            m_SSAONoise = new Texture();
            m_SSAONoise->Generate(4, 4, GL_RGBA16F, GL_RGB, GL_HALF_FLOAT, &ssaoNoise[0]);

            m_SSAOShader->SetVectorArray("kernel", ssaoKernel.size(), ssaoKernel);
            m_SSAOShader->SetInt("sampleCount", SSAOKernelSize);
            //for (int i = 0; i < SSAOKernelSize; ++i)
            //{
                // TODO(Joey): make Set...Array calls for shader
                //m_SSAOShader->SetVector("kernel[" + std::to_string(i) + "]", ssaoKernel[i]);
            //}
        }

        // bloom 
        {
            m_BloomRenderTarget = new RenderTarget(1024, 1024, GL_HALF_FLOAT, 1, false);
            BloomOutput = m_BloomRenderTarget->GetColorTexture(0);

            m_BloomShader = Cell::Resources::LoadShader("bloom", "shaders/screen_quad.vs", "shaders/post/bloom.fs");
            m_SSAOShader->Use();
            m_SSAOShader->SetInt("HDRScene", 0);
        }
    }
    // --------------------------------------------------------------------------------------------
    PostProcessor::~PostProcessor()
    {
        delete m_GaussianRT512_H;
        delete m_GaussianRT512_V;
        delete m_GaussianRT256_H;
        delete m_GaussianRT256_V;
        delete m_GaussianRT128_H;
        delete m_GaussianRT128_V;
        delete m_GaussianRT64_H;
        delete m_GaussianRT64_V;
        delete m_SSAONoise;
        delete m_SSAORenderTarget;
        delete m_BloomRenderTarget;
    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::ProcessPreLighting(Renderer* renderer, RenderTarget* gBuffer, Camera* camera)
    {
        // ssao
        if (SSAO)
        {
            gBuffer->GetColorTexture(0)->Bind(0);
            gBuffer->GetColorTexture(1)->Bind(1);
            m_SSAONoise->Bind(2);

            m_SSAOShader->Use();
            m_SSAOShader->SetVector("renderSize", renderer->GetRenderSize());
            m_SSAOShader->SetMatrix("projection", camera->Projection);
            m_SSAOShader->SetMatrix("view", camera->View);

            glBindFramebuffer(GL_FRAMEBUFFER, m_SSAORenderTarget->ID);
            glViewport(0, 0, m_SSAORenderTarget->Width, m_SSAORenderTarget->Height);
            glClear(GL_COLOR_BUFFER_BIT);
            renderer->renderMesh(renderer->m_NDCPlane, m_SSAOShader);
        }
    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::ProcessPostLighting(Renderer* renderer, RenderTarget* output, Camera* camera)
    {
        // bloom
        if (Bloom)
        {
            m_BloomShader->Use();
            output->GetColorTexture(0)->Bind(0);

            glBindFramebuffer(GL_FRAMEBUFFER, m_BloomRenderTarget->ID);
            glViewport(0, 0, m_BloomRenderTarget->Width, m_BloomRenderTarget->Height);
            glClear(GL_COLOR_BUFFER_BIT);
            renderer->renderMesh(renderer->m_NDCPlane, m_BloomShader);

            // blur bloom result
            Texture* output;
            output = blur(renderer, m_BloomRenderTarget->GetColorTexture(0), 512, 2, 1.0f);
            output = blur(renderer, output, 256, 2, 1.0f);
            output = blur(renderer, output, 128, 2, 1.0f);
            output = blur(renderer, output, 64, 2, 1.0f);
            BloomOutput = output;
        }
    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::Blit(Renderer* renderer, Texture* source)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, renderer->GetRenderSize().x, renderer->GetRenderSize().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // bind input texture data
        source->Bind(0);
        BloomOutput->Bind(1);

        // set settings 
        // TODO(Joey): only update settings when changed
        m_PostProcessShader->Use();
        m_PostProcessShader->SetBool("Sepia", Sepia);
        m_PostProcessShader->SetBool("Vignette", Vignette);
        m_PostProcessShader->SetBool("Bloom", Bloom);

        renderer->renderMesh(renderer->m_NDCPlane, m_PostProcessShader);
    }
    // --------------------------------------------------------------------------------------------
    Texture* PostProcessor::blur(Renderer* renderer, Texture* src, int renderSize, int count, float range)
    {
        assert(count >= 2 && count % 2 == 0); // count must be more than 2 and be even               

        // pick pre-defined render targets for blur based on render size
        // TODO: use renderSize as heuristic, or make pre-defined options
        glViewport(0, 0, renderSize, renderSize); // TODO: make this resolution-agnostic (now breaks with 2 different resolutions)
        RenderTarget *rtHorizontal;
        RenderTarget *rtVertical;
        if (renderSize == 512)
        {
            rtHorizontal = m_GaussianRT512_H;
            rtVertical = m_GaussianRT512_V;
        } 
        else if (renderSize == 256)
        {
            rtHorizontal = m_GaussianRT256_H;
            rtVertical = m_GaussianRT256_V;
        } 
        else if (renderSize == 128)
        {
            rtHorizontal = m_GaussianRT128_H;
            rtVertical = m_GaussianRT128_V;
        }
        else 
        {
            rtHorizontal = m_GaussianRT64_H;
            rtVertical = m_GaussianRT64_V;
        }

        bool horizontal = true;
        m_OnePassGaussianShader->Use();
        m_OnePassGaussianShader->SetFloat("range", range);        
        for (int i = 0; i < count; ++i, horizontal = !horizontal)
        {
            m_OnePassGaussianShader->SetBool("horizontal", horizontal);
            if (i == 0)
            {
                src->Bind(0);
            } 
            else if(horizontal)
            {
                rtVertical->GetColorTexture(0)->Bind(0);
            }
            else if (!horizontal)
            {
                rtHorizontal->GetColorTexture(0)->Bind(0);
            }
            glBindFramebuffer(GL_FRAMEBUFFER, horizontal ? rtHorizontal->ID : rtVertical->ID);
            renderer->renderMesh(renderer->m_NDCPlane, m_OnePassGaussianShader);
        }

        // output resulting (blurred) texture
        return rtVertical->GetColorTexture(0); 
    }
}