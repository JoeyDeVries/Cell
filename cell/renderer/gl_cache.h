#ifndef CELL_GL_CACHE_H
#define CELL_GL_CACHE_H

#include "../glad/glad.h"

namespace Cell
{
    /* 

      GLCache stores the latest state of relevant OpenGL state and through the use of public 
      setters it only updates the actual OpenGL state when the value is different than before.

      Switching GL state (like shaders, depth test, blend state) can be quite expensive. By 
      propagating every state change through GLCache we prevent unnecessary state changes.

    */
    class GLCache
    {
    private:
        // gl toggles
        bool m_DepthTest;
        bool m_Blend;
        bool m_CullFace;

        // gl state
        GLenum m_DepthFunc;
        GLenum m_BlendSrc;
        GLenum m_BlendDst;
        GLenum m_FrontFace;
        GLenum m_PolygonMode;

        // shaders
        unsigned int m_ActiveShaderID;
    public:
        GLCache();
        ~GLCache();

        // update GL state if requested state is different from current GL state.
        void SetDepthTest(bool enable);
        void SetDepthFunc(GLenum depthFunc);
        void SetBlend(bool enable);
        void SetBlendFunc(GLenum src, GLenum dst);
        void SetCull(bool enable);
        void SetCullFace(GLenum face);
        void SetPolygonMode(GLenum mode);

        // switch shader only if a different ID is requested.
        // note that we won't use this too often, as we already sort render state on shader ID.
        void SwitchShader(unsigned int ID);
    };
}
#endif