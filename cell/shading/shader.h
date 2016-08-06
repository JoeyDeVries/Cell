#ifndef CELL_SHADING_SHADER_H
#define CELL_SHADING_SHADER_H

#include <string>

/* NOTE(Joey):

  asd

*/
class Shader
{
public:

private:
    unsigned int m_ID;


public:
    Shader(std::string vsPath, std::string fsPath);

    void Use();

private:
    // TODO(Joey): move all shader loading code to specialized loader
    std::string readShader(std::ifstream &file, std::string directory);
};

#endif