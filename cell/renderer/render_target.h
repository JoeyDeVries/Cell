#ifndef CELL_RENDERER_RENDER_TARGET_H
#define CELL_RENDERER_RENDER_TARGET_H

#include "renderer.h"
#include "../shading/texture.h"

#include <GL/glew.h>

#include <vector>

namespace Cell
{
    class RenderTarget
    {
        friend Renderer;
    public:
        unsigned int Width;
        unsigned int Height;

        bool HasDepthAndStencil;
    private:
        unsigned int m_ID;
        GLenum       m_Target = GL_TEXTURE_2D;

        Texture              m_DepthStencil;
        std::vector<Texture> m_ColorAttachments;
    public:
        RenderTarget(unsigned int width, unsigned int height, GLenum type = GL_UNSIGNED_BYTE, unsigned int nrColorAttachments = 1, bool depthAndStencil = true);


        // NOTE(Joey): we don't expose bind functionality as this is handled 
        // internally at the renderer SetRenderTarget due to the delayed
        // render approach.
        // void Bind();

        Texture *GetDepthStencilTexture();
        Texture *GetColorTexture(unsigned int index);

        void SetTarget(GLenum target);
    };
}

#endif