#ifndef CELL_SHADING_SHADER_H
#define CELL_SHADING_SHADER_H

#include <string>
#include <vector>

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/matrix.h>

namespace Cell
{



    /* NOTE(Joey):

      asd

    */
    class Shader
    {
    public:
        // TODO(Joey): move these in their own header file
        // but first determine their proper usage before
        // isolating them.
        enum SHADER_TYPE
        {
            SHADER_TYPE_BOOL,
            SHADER_TYPE_INT,
            SHADER_TYPE_FLOAT,
            SHADER_TYPE_SAMPLER1D,
            SHADER_TYPE_SAMPLER2D,
            SHADER_TYPE_SAMPLER3D,
            SHADER_TYPE_VEC2,
            SHADER_TYPE_VEC3,
            SHADER_TYPE_VEC4,
            SHADER_TYPE_MAT2,
            SHADER_TYPE_MAT3,
            SHADER_TYPE_MAT4,
        };

        struct Uniform
        {
            SHADER_TYPE Type;
            std::string Name;
            int Size;
            unsigned int Location;
        };

        struct VertexAttribute
        {
            SHADER_TYPE Type;
            std::string Name;
            int Size;
            unsigned int Location;
        };

        std::vector<Uniform>         Uniforms;
        std::vector<VertexAttribute> Attributes;
    private:
        unsigned int m_ID;

    public:
        Shader();
        Shader(std::string vsPath, std::string fsPath);

        void Load(std::string vsPath, std::string fsPath);

        void Use();

        void SetInt   (std::string location, int   value);
        void SetBool  (std::string location, bool  value);
        void SetFloat (std::string location, float value);
        void SetVector(std::string location, math::vec2  value);
        void SetVector(std::string location, math::vec3  value);
        void SetVector(std::string location, math::vec4  value);
        void SetMatrix(std::string location, math::mat2  value);
        void SetMatrix(std::string location, math::mat3  value);
        void SetMatrix(std::string location, math::mat4  value);
        //void SetTexture1D();
        //void SetTexture2D();
        //void SetTexture3D();

    private:
        // TODO(Joey): move all shader loading code to specialized loader
        std::string readShader(std::ifstream &file, std::string directory);

        // NOTE(Joey): retrieves uniform location from pre-stored uniform locations and
        // reports an error if a non-uniform is set.
        unsigned int getUniformLocation(std::string name);
    };
}
#endif