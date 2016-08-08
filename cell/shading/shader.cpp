#include "shader.h"

#include <fstream>

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

        // NOTE(Joey): parse shader sources to retrieve all uniforms in use (with their respective locations)
        //ShaderParser parser;
        //parser.Parse(vsSource);
        //parser.Parse(fsSource);
        // TODO(Joey): I'm not sure we need a specific new class for parsing a shader, we'd probably want to
        // do this in the future in a specialized shader loader anyways; so might as well do it here for now.
        // NOTE(Joey): nevermind, we can query the (active, and thus not optimized) uniform/attribute state
        // from OpenGL itself: see http://docs.gl/gl3/glGetProgram w/ pname 'GL_ACTIVE_UNIFORMS', 
        // 'GL_ACTIVE_ATTRIBUTES' and use these counts to query the items directly w/ glGetActiveAttrib
        // and glGetActiveUniform

        vsFile.close();
        fsFile.close();
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


    void Shader::Use()
    {
        glUseProgram(m_ID);
    }
}