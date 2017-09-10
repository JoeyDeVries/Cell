#ifndef CELL_RENDERER_RENDER_TARGET_H
#define CELL_RENDERER_RENDER_TARGET_H

#include "renderer.h"
#include "../shading/texture.h"

#include "../glad/glad.h"

#include <vector>

namespace Cell
{
    class RenderTarget
    {
        friend Renderer;
    public:
        unsigned int ID;

        unsigned int Width;
        unsigned int Height;
        GLenum       Type;

        bool HasDepthAndStencil;
    private:
        GLenum       m_Target = GL_TEXTURE_2D;
        Texture              m_DepthStencil;
        std::vector<Texture> m_ColorAttachments;
    public:
        RenderTarget(unsigned int width, unsigned int height, GLenum type = GL_UNSIGNED_BYTE, unsigned int nrColorAttachments = 1, bool depthAndStencil = true);

        Texture *GetDepthStencilTexture();
        Texture *GetColorTexture(unsigned int index);

        void Resize(unsigned int width, unsigned int height);
        void SetTarget(GLenum target);

        // TODO(Joey): this shouldn't be called by end-users, come up with something else than private/friend
        //void Bind(bool clear = true, bool setViewport = true);
    };
}

#endif