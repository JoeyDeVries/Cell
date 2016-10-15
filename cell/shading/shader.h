#ifndef CELL_SHADING_SHADER_H
#define CELL_SHADING_SHADER_H

#include <string>
#include <vector>

#include <math/linear_algebra/vector.h>
#include <math/linear_algebra/matrix.h>

#include "shading_types.h"

namespace Cell
{



    /* NOTE(Joey):

      asd

    */
    class Shader
    {
    public:
        unsigned int ID;

        std::vector<Uniform>         Uniforms;
        std::vector<VertexAttribute> Attributes;

    public:
        Shader();
        Shader(std::string name, std::string vsCode, std::string fsCode, std::vector<std::string> defines = std::vector<std::string>());

        void Load(std::string name, std::string vsCode, std::string fsCode, std::vector<std::string> defines = std::vector<std::string>());

        void Use();

        bool HasUniform(std::string name);

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
        // NOTE(Joey): retrieves uniform location from pre-stored uniform locations and
        // reports an error if a non-uniform is set.
        int getUniformLocation(std::string name);
    };
}
#endif