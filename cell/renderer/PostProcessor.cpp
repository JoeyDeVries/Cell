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
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator) * 2.0 - 1.0,
                    randomFloats(generator)
                );
                sample = math::normalize(sample);
                sample *= randomFloats(generator);
                float scale = (float)i / (float)SSAOKernelSize;
                scale = math::lerp(0.1f, 1.0f, scale * scale);
                sample *= scale;
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

            for (int i = 0; i < SSAOKernelSize; ++i)
            {
                // TODO(Joey): make Set...Array calls for shader
                m_SSAOShader->SetVector("kernel[" + std::to_string(i) + "]", ssaoKernel[i]);
            }
        }

        // bloom
        {

        }
    }
    // --------------------------------------------------------------------------------------------
    PostProcessor::~PostProcessor()
    {
        delete m_SSAONoise;
    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::ProcessPreLighting(Renderer* renderer, RenderTarget* gBuffer, Camera* camera)
    {
        // ssao
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
    // --------------------------------------------------------------------------------------------
    void PostProcessor::Blit(Renderer* renderer, Texture* source)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, renderer->GetRenderSize().x, renderer->GetRenderSize().y);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        // bind input texture data
        source->Bind(0);

        // set settings 
        // TODO(Joey): only update settings when changed
        m_PostProcessShader->Use();
        m_PostProcessShader->SetBool("Sepia", Sepia);
        m_PostProcessShader->SetBool("Vignette", Vignette);

        renderer->renderMesh(renderer->m_NDCPlane, m_PostProcessShader);
    }
}