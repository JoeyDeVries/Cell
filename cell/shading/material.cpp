#include "material.h"

namespace Cell
{

    void Material::SetBool(std::string name, bool value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_BOOL;
        m_Uniforms[name].Bool = value;
    }
    void Material::SetInt(std::string name, int value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_INT;
        m_Uniforms[name].Int  = value;
    }
    void Material::SetFloat(std::string name, float value)
    {
        m_Uniforms[name].Type  = SHADER_TYPE_FLOAT;
        m_Uniforms[name].Float = value;
    }
    void Material::SetTexture(std::string name, Texture *value, unsigned int unit)
    {
        m_SamplerUniforms[name].Unit = unit;
        m_SamplerUniforms[name].Value = value;

        switch (value->Type)
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
        
        if (Shader)
        {
            Shader->Use();
            Shader->SetInt(name, unit);
        }
    }
    void Material::SetVector(std::string name, math::vec2 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC2;
        m_Uniforms[name].Vec2 = value;
    }
    void Material::SetVector(std::string name, math::vec3 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC3;
        m_Uniforms[name].Vec3 = value;
    }
    void Material::SetVector(std::string name, math::vec4 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_VEC4;
        m_Uniforms[name].Vec4 = value;
    }
    void Material::SetMatrix(std::string name, math::mat2 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT2;
        m_Uniforms[name].Mat2 = value;
    }
    void Material::SetMatrix(std::string name, math::mat3 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT3;
        m_Uniforms[name].Mat3 = value;
    }
    void Material::SetMatrix(std::string name, math::mat4 value)
    {
        m_Uniforms[name].Type = SHADER_TYPE_MAT4;
        m_Uniforms[name].Mat4 = value;
    }

    std::map<std::string, UniformValue>*        Material::GetUniforms()
    {
        return &m_Uniforms;
    }

    std::map<std::string, UniformValueSampler>* Material::GetSamplerUniforms()
    {
        return &m_SamplerUniforms;
    }
}