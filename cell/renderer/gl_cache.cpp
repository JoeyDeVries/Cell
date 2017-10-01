#include "gl_cache.h"

namespace Cell
{
    GLCache::GLCache()
    {

    }
    // --------------------------------------------------------------------------------------------
    GLCache::~GLCache()
    {

    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetDepthTest(bool enable)
    {
        if (m_DepthTest != enable)
        {
            m_DepthTest = enable;
            if (enable)
                glEnable(GL_DEPTH_TEST);
            else
                glDisable(GL_DEPTH_TEST);
        }

    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetDepthFunc(GLenum depthFunc)
    {
        if (m_DepthFunc != depthFunc)
        {
            m_DepthFunc = depthFunc;
            glDepthFunc(depthFunc);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetBlend(bool enable)
    {
        if (m_Blend != enable)
        {
            m_Blend = enable;
            if(enable)
                glEnable(GL_BLEND);
            else
                glDisable(GL_BLEND);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetBlendFunc(GLenum src, GLenum dst)
    {
        if (m_BlendSrc != src || m_BlendDst != dst)
        {
            m_BlendSrc = src;
            m_BlendDst = dst;
            glBlendFunc(src, dst);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetCull(bool enable)
    {
        if (m_CullFace != enable)
        {
            m_CullFace = enable;
            if(enable)
                glEnable(GL_CULL_FACE);
            else
                glDisable(GL_CULL_FACE);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetCullFace(GLenum face)
    {
        if (m_FrontFace != face)
        {
            m_FrontFace = face;
            glCullFace(face);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SetPolygonMode(GLenum mode)
    {
        if (m_PolygonMode != mode)
        {
            m_PolygonMode = mode;
            glPolygonMode(GL_FRONT_AND_BACK, mode);
        }
    }
    // --------------------------------------------------------------------------------------------
    void GLCache::SwitchShader(unsigned int ID)
    {
        if (m_ActiveShaderID != ID)
        {
            glUseProgram(ID);
        }
    }
}