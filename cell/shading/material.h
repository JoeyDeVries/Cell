#ifndef CELL_SHADING_MATERIAL_H
#define CELL_SHADING_MATERIAL_H

#include <GL/glew.h>

#include <map>

#include <math/math.h>

#include "shading_types.h"
#include "shader.h"
#include "texture.h"
#include "texture_cube.h"

namespace Cell
{
    // TODO(Joey): keep track of a 'dirty' flag for each material, and only update the shader in
    // the shader routines whenever the 'dirty' flag is set; otherwise ignore and keep current setting.
    /* NOTE(Joey):

      asd

    */
    class Material
    {
    private:
        // shader state
        Shader *m_Shader;
        std::map<std::string, UniformValue>        m_Uniforms;
        std::map<std::string, UniformValueSampler> m_SamplerUniforms; // NOTE(Joey): process samplers differently 
    public:
        math::vec4 Color = math::vec4(1.0f);

        // depth state
        bool   DepthTest    = true;
        bool   DepthWrite   = true;
        GLenum DepthCompare = GL_LESS;

        // stencil state

        // face culling state
        bool   Cull             = false;
        GLenum CullFace         = GL_BACK;
        GLenum CullWindingOrder = GL_CCW;

        // blending state
        bool   Blend           = false;
        GLenum BlendSrc      = GL_SRC_ALPHA;
        GLenum BlendDst      = GL_ONE_MINUS_SRC_ALPHA;
        GLenum BlendEquation = GL_FUNC_ADD;

    private:
      
    public:
        Material();
        Material(Shader *shader);

        Shader* GetShader();
        void    SetShader(Shader *shader);

        void SetBool(std::string name,        bool value);
        void SetInt(std::string name,         int value);
        void SetFloat(std::string name,       float value);
        void SetTexture(std::string name,     Texture *value, unsigned int unit = 0);
        void SetTextureCube(std::string name, TextureCube *value, unsigned int unit = 0);
        void SetVector(std::string name,      math::vec2 value);
        void SetVector(std::string name,      math::vec3 value);
        void SetVector(std::string name,      math::vec4 value);
        void SetMatrix(std::string name,      math::mat2 value);
        void SetMatrix(std::string name,      math::mat3 value);
        void SetMatrix(std::string name,      math::mat4 value);

        std::map<std::string, UniformValue>*        GetUniforms();
        std::map<std::string, UniformValueSampler>* GetSamplerUniforms();
    };
}
#endif