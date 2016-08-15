#ifndef CELL_SHADING_MATERIAL_H
#define CELL_SHADING_MATERIAL_H

#include <GL/glew.h>

#include <math/math.h>

#include "shader.h"
#include "texture.h"

namespace Cell
{
    /* NOTE(Joey):

      asd

    */
    class Material
    {
    public:
        Shader *Shader;

        // lighting
        Texture *Albedo;
        Texture *Normal;
        Texture *Specular;
        Texture *Occlusion;

        math::vec4 Color;

        // depth state
        bool DepthTest;
        bool DepthWrite;
        GLenum DepthCompare;

        // blending state
        bool Blend;
        GLenum BlendSrc;
        GLenum BlendDst;
        GLenum BlendEquation;

        // face culling state
        bool Cull;
        GLenum CullWindingOrder;
        GLenum CullFace;
    private:

    public:
    };
}
#endif