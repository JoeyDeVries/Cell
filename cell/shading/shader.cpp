#include "shader.h"

#include <fstream>
#include <string>

#include <gl/glew.h>

#include <utility/logging/log.h>

#include "shader_parser.h"

namespace Cell
{
    Shader::Shader(std::string vsPath, std::string fsPath)
    {
        std::ifstream vsFile, fsFile;
        vsFile.open(vsPath);
        fsFile.open(fsPath);

        // NOTE(Joey): if either of the two files don't exist, return w/ error message
        if (!vsFile.is_open() || !fsFile.is_open())
        {
            Log::Message("Shader failed to load at path: " + vsPath + " and " + fsPath, LOG_ERROR);
            return;
        }

        // NOTE(Joey): retrieve directory (for relative paths in shader includes)
        std::string directory = vsPath.substr(0, vsPath.find_last_of("/\\"));
        std::string vsSource = readShader(vsFile, directory);
        std::string fsSource = readShader(fsFile, directory);

        // NOTE(Joey): compile both shaders and link them
        unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
        unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
        m_ID = glCreateProgram();
        int status;
        char log[512];

        const char *vsSourceC = vsSource.c_str();
        const char *fsSourceC = fsSource.c_str();
        glShaderSource(vs, 1, &vsSourceC, NULL);
        glShaderSource(fs, 1, &fsSourceC, NULL);
        glCompileShader(vs);
        glCompileShader(fs);

        glGetShaderiv(vs, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(vs, 512, NULL, log);
            Log::Message("Vertex shader compilation error at: " + vsPath + "\n" + std::string(log), LOG_ERROR);
        }
        glGetShaderiv(fs, GL_COMPILE_STATUS, &status);
        if (!status)
        {
            glGetShaderInfoLog(fs, 512, NULL, log);
            Log::Message("Fragment shader compilation error at: " + fsPath + "\n" + std::string(log), LOG_ERROR);
        }

        glAttachShader(m_ID, vs);
        glAttachShader(m_ID, fs);
        glLinkProgram(m_ID);

        glGetProgramiv(m_ID, GL_LINK_STATUS, &status);
        if (!status)
        {
            glGetProgramInfoLog(m_ID, 512, NULL, log);
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

        vsFile.close();
        fsFile.close();
    }

    void Shader::Use()
    {
        glUseProgram(m_ID);
    }

    void Shader::SetInt(std::string location, int value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform1i(loc, value);
    }
    void Shader::SetBool(std::string location, bool value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform1i(loc, (int)value);
    }
    void Shader::SetFloat(std::string location, float value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform1f(loc, value);
    }
    void Shader::SetVector(std::string location, math::vec2 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform2fv(loc, 1, &value[0]);
    }
    void Shader::SetVector(std::string location, math::vec3 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform3fv(loc, 1, &value[0]);
    }
    void Shader::SetVector(std::string location, math::vec4 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniform4fv(loc, 1, &value[0]);
    }
    void Shader::SetMatrix(std::string location, math::mat2 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniformMatrix2fv(loc, 1, GL_FALSE, &value[0][0]);
    }
    void Shader::SetMatrix(std::string location, math::mat3 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
    }
    void Shader::SetMatrix(std::string location, math::mat4 value)
    {
        unsigned int loc = getUniformLocation(location);
        if (loc)
            glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
    }

    std::string Shader::readShader(std::ifstream &file, std::string directory)
    {
        std::string source, line;
        while (std::getline(file, line))
        {
            // NOTE(Joey): if we encounter an #include line, include other shader source
            if (line.substr(0, 8) == "#include")
            {
                // TODO(Joey): write a trim function that trims any character (trim whitespace and ;)
                std::string includePath = directory + "/" + line.substr(9);
                std::ifstream includeFile(includePath);
                if (includeFile.is_open())
                {
                    // NOTE(Joey): we recursively read the shader file to support any shader include depth
                    source += readShader(includeFile, directory);
                }
                else
                {
                    Log::Message("Shader include: " + includePath + " failed to open.", LOG_ERROR);
                }
            }
            else
                source += line + "\n";
        }
        return source;
    }

    unsigned int Shader::getUniformLocation(std::string name)
    {
        // TODO(Joey): read from uniform/attribute array as originally obtained from OpenGL
        return 0;
    }


}