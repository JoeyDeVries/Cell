#ifndef CELL_SHADING_SHADER_H
#define CELL_SHADING_SHADER_H

// NOTE(Joey): see renderer.h for why we do this:
class string;
class vec4; class vec3; class vec2;
class mat4; class mat3; class mat2;

namespace Cell
{
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

        void SetInt   (std::string location, int   value);
        void SetBool  (std::string location, bool  value);
        void SetFloat (std::string location, float value);
        void SetVector(std::string location, vec2  value);
        void SetVector(std::string location, vec3  value);
        void SetVector(std::string location, vec4  value);
        void SetMatrix(std::string location, mat2  value);
        void SetMatrix(std::string location, mat3  value);
        void SetMatrix(std::string location, mat4  value);
        //void SetTexture1D();
        //void SetTexture2D();
        //void SetTexture3D();

    private:
        // TODO(Joey): move all shader loading code to specialized loader
        std::string readShader(std::ifstream &file, std::string directory);

        // NOTE(Joey): retrieves uniform location from pre-stored uniform locations and
        // reports an error if a non-uniform is set.
        unsigned int getUniformLocation(string name);
    };
}
#endif