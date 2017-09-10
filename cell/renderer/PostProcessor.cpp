#include "PostProcessor.h"

#include "render_target.h"
#include "renderer.h"

#include "../resources/resources.h"
#include "../shading/texture.h"
#include "../shading/shader.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    PostProcessor::PostProcessor()
    {
        m_PostProcessShader = Cell::Resources::LoadShader("post process", "shaders/screen_quad.vs", "shaders/post_processing.fs");
        m_PostProcessShader->Use();
        m_PostProcessShader->SetInt("TexSrc", 0);
    }
    // --------------------------------------------------------------------------------------------
    PostProcessor::~PostProcessor()
    {

    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::ProcessPreLighting(Renderer* renderer, RenderTarget* gBuffer)
    {

    }
    // --------------------------------------------------------------------------------------------
    void PostProcessor::ProcessPostLighting(Renderer* renderer, RenderTarget* renderOutput)
    {

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
        m_PostProcessShader->SetBool("vignette", true);

        renderer->renderMesh(renderer->m_NDCPlane, m_PostProcessShader);
    }
}