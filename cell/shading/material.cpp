#include "material.h"

#include "../resources/resources.h"

namespace Cell
{
    // --------------------------------------------------------------------------------------------
    Material::Material()
    {

    }
    // --------------------------------------------------------------------------------------------
    Material::Material(Shader* shader)
    {
        m_Shader = shader;
    }
    // --------------------------------------------------------------------------------------------
    Shader* Material::GetShader()
    {
        return m_Shader;
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetShader(Shader* shader)
    {
        m_Shader = shader;
    }
    // --------------------------------------------------------------------------------------------
    Material Material::Copy()
    {
        Material copy(m_Shader);

        copy.Type = Type;

        copy.Color = Color;
        
        copy.DepthTest = DepthTest;
        copy.DepthWrite = DepthWrite;
        copy.DepthCompare = DepthCompare;

        copy.Cull = Cull;
        copy.CullFace = CullFace;
        copy.CullWindingOrder = CullWindingOrder;

        copy.Blend = Blend;
        copy.BlendSrc = BlendSrc;
        copy.BlendDst = BlendDst;
        copy.BlendEquation = BlendEquation;

        copy.m_Uniforms = m_Uniforms;
        copy.m_SamplerUniforms = m_SamplerUniforms;
        
        return copy;
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetBool(std::string name, bool value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_BOOL;
        m_Uniforms[name].Bool = value;
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetInt(std::string name, int value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_INT;
        m_Uniforms[name].Int  = value;
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetFloat(std::string name, float value)
    {
        m_Uniforms[name].Type  = SHADER_TYPE_FLOAT;
        m_Uniforms[name].Float = value;
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetTexture(std::string name, Texture* value, unsigned int unit)
    {
        m_SamplerUniforms[name].Unit    = unit;
        m_SamplerUniforms[name].Texture = value;

        switch (value->Target)
        {
        case GL_TEXTURE_1D:
            m_SamplerUniforms[name].Type      = SHADER_TYPE_SAMPLER1D;
            break;
        case GL_TEXTURE_2D:
            m_SamplerUniforms[name].Type      = SHADER_TYPE_SAMPLER2D;
            break;
        case GL_TEXTURE_3D:
            m_SamplerUniforms[name].Type      = SHADER_TYPE_SAMPLER3D;
            break;
        case GL_TEXTURE_CUBE_MAP:
            m_SamplerUniforms[name].Type        = SHADER_TYPE_SAMPLERCUBE;
            break;
        }
        
        if (m_Shader)
        {
            m_Shader->Use();
            m_Shader->SetInt(name, unit);
        }
    }
    // --------------------------------------------------------------------------------------------
    void Material::SetTextureCube(std::string name, TextureCube* value, unsigned int unit)
    {
        m_SamplerUniforms[name].Unit        = unit;
        m_SamplerUniforms[name].Type        = SHADER_TYPE_SAMPLERCUBE;
        m_SamplerUniforms[name].TextureCube = value;

        if (m_Shader)
        {
            m_Shader->Use();
            m_Shader->SetInt(name, unit);
        }
    }
    // ------------------------------------------------------------------------
    void Material::SetVector(std::string name, math::vec2 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC2;
        m_Uniforms[name].Vec2 = value;
    }
    // ------------------------------------------------------------------------
    void Material::SetVector(std::string name, math::vec3 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC3;
        m_Uniforms[name].Vec3 = value;
    }
    // ------------------------------------------------------------------------
    void Material::SetVector(std::string name, math::vec4 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC4;
        m_Uniforms[name].Vec4 = value;
    }
    // ------------------------------------------------------------------------
    void Material::SetMatrix(std::string name, math::mat2 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT2;
        m_Uniforms[name].Mat2 = value;
    }
    // ------------------------------------------------------------------------
    void Material::SetMatrix(std::string name, math::mat3 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT3;
        m_Uniforms[name].Mat3 = value;
    }
    // ------------------------------------------------------------------------
    void Material::SetMatrix(std::string name, math::mat4 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT4;
        m_Uniforms[name].Mat4 = value;
    }
    // ------------------------------------------------------------------------
    std::map<std::string, UniformValue>*        Material::GetUniforms()
    {
        return &m_Uniforms;
    }
    // ------------------------------------------------------------------------
    std::map<std::string, UniformValueSampler>* Material::GetSamplerUniforms()
    {
        return &m_SamplerUniforms;
    }
}