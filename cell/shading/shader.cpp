#include "shader.h"

#include <utility/logging/log.h>

#include <gl/glew.h>

#include <fstream>
#include <string>

namespace Cell
{
    // ------------------------------------------------------------------------
    Shader::Shader()
    {

    }
    // ------------------------------------------------------------------------
    Shader::Shader(std::string vsCode, std::string fsCode)
    {      
        Load(vsCode, fsCode);
    }
    // ------------------------------------------------------------------------
    void Shader::Load(std::string vsCode, std::string fsCode)
    {
        // NOTE(Joey): compile both shaders and link them
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        m_ID = glCreateProgram();
        int status;
        char log[1024];

        const char *vsSourceC = vsCode.c_str();
        const char *fsSourceC = fsCode.c_str();
        glShaderSource(vs, 1, &vsSourceC, NULL);
        glShaderSource(fs, 1, &fsSourceC, NULL);
        glCompileShader(vs);
        glCompileShader(fs);

        glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(vs, 1024, NULL, log);
            Log::Message("Vertex shader compilation error at: TODO: GET SHADER NAME HERE!\n" + std::string(log), LOG_ERROR);
        }
        glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(fs, 1024, NULL, log);
            Log::Message("Fragment shader compilation error at: TODO: GET SHADER NAME HERE!\n" + std::string(log), LOG_ERROR);
        }

        glAttachShader(m_ID, vs);
        glAttachShader(m_ID, fs);
        glLinkProgram(m_ID);

        glGetProgramiv(m_ID, GL_LINK_STATUS, &status);
        if (!status)
        {
            glGetProgramInfoLog(m_ID, 1024, NULL, log);
            Log::Message("Shader program linking error: \n" + std::string(log), LOG_ERROR);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);

        // NOTE(Joey): query the number of active uniforms and attributes
        int nrAttributes, nrUniforms;
        glGetProgramiv(m_ID, GL_ACTIVE_ATTRIBUTES, &nrAttributes);
        glGetProgramiv(m_ID, GL_ACTIVE_UNIFORMS, &nrUniforms);
        Attributes.resize(nrAttributes);
        Uniforms.resize(nrUniforms);

        // NOTE(Joey): iterate over all active attributes
        char buffer[128];
        for (unsigned int i = 0; i < nrAttributes; ++i)
        {
            GLenum glType;
            glGetActiveAttrib(m_ID, i, sizeof(buffer), 0, &Attributes[i].Size, &glType, buffer);
            Attributes[i].Name = std::string(buffer);
            Attributes[i].Type = SHADER_TYPE_BOOL; // TODO(Joey): think of clean way to manage type conversions of OpenGL and custom type

            Attributes[i].Location = glGetAttribLocation(m_ID, buffer);
        }

        // NOTE(Joey): iterate over all active uniforms
        for (unsigned int i = 0; i < nrUniforms; ++i)
        {
            GLenum glType;
            glGetActiveUniform(m_ID, i, sizeof(buffer), 0, &Uniforms[i].Size, &glType, buffer);
            Uniforms[i].Name = std::string(buffer);
            Uniforms[i].Type = SHADER_TYPE_BOOL;  // TODO(Joey): think of clean way to manage type conversions of OpenGL and custom type

            Uniforms[i].Location = glGetUniformLocation(m_ID, buffer);
        }
    }
    // ------------------------------------------------------------------------
    void Shader::Use()
    {
        glUseProgram(m_ID);
    }
    // ------------------------------------------------------------------------
    bool Shader::HasUniform(std::string name)
    {
        for (unsigned int i = 0; i < Uniforms.size(); ++i)
        {
            if(Uniforms[i].Name == name)
                return true;
        }
        return false;
    }
    // ------------------------------------------------------------------------
    void Shader::SetInt(std::string location, int value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform1i(loc, value);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetBool(std::string location, bool value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform1i(loc, (int)value);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetFloat(std::string location, float value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform1f(loc, value);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVector(std::string location, math::vec2 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform2fv(loc, 1, &value[0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVector(std::string location, math::vec3 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform3fv(loc, 1, &value[0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetVector(std::string location, math::vec4 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniform4fv(loc, 1, &value[0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMatrix(std::string location, math::mat2 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniformMatrix2fv(loc, 1, GL_FALSE, &value[0][0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMatrix(std::string location, math::mat3 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    void Shader::SetMatrix(std::string location, math::mat4 value)
    {
        int loc = getUniformLocation(location);
        if (loc >= 0)
            glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
        else
            Log::Message("Shader uniform at location: " + location + " not found in shader.", LOG_WARNING);
    }
    // ------------------------------------------------------------------------
    int Shader::getUniformLocation(std::string name)
    {
        // TODO(Joey): read from uniform/attribute array as originally obtained from OpenGL
        for (unsigned int i = 0; i < Uniforms.size(); ++i)
        {
            if(Uniforms[i].Name == name)
                return Uniforms[i].Location;
        }
        return -1;
    }


}