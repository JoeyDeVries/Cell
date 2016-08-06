#ifndef CELL_SHADING_SHADER_PARSER_H
#define CELL_SHADING_SHADER_PARSER_H

#include <vector>
#include <string>

/* NOTE(Joey):

  asd

*/
class ShaderParser
{
public:
    std::vector<std::string> m_Attributes;
    std::vector<std::string> m_Uniforms;
    std::vector<std::string> m_Samplers;
    
    std::string Preprocess(std::string shaderSource);

    std::string Parse(std::string shadserSource);
};


#endif