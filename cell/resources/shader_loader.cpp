#include "shader_loader.h"

#include <utility/logging/log.h> 

namespace Cell
{
    // ------------------------------------------------------------------------
    Shader ShaderLoader::Load(std::string vsPath, std::string fsPath, std::vector<std::string> defines)
    {
        std::ifstream vsFile, fsFile;
        vsFile.open(vsPath);
        fsFile.open(fsPath);

        // NOTE(Joey): if either of the two files don't exist, return w/ error message
        if (!vsFile.is_open() || !fsFile.is_open())
        {
            Log::Message("Shader failed to load at path: " + vsPath + " and " + fsPath, LOG_ERROR);
            return Shader();
        }

        // NOTE(Joey): retrieve directory (for relative paths in shader includes)
        std::string directory = vsPath.substr(0, vsPath.find_last_of("/\\"));
        std::string vsSource = readShader(vsFile, directory);
        std::string fsSource = readShader(fsFile, directory);

        // NOTE(Joey): now build the shader with the source code
        Shader shader(vsSource, fsSource, defines);

        vsFile.close();
        fsFile.close();

        return shader;
    }
    // ------------------------------------------------------------------------
    std::string ShaderLoader::readShader(std::ifstream &file, std::string directory)
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
                includeFile.close();
            }
            else
                source += line + "\n";
        }
        return source;
    }


}
