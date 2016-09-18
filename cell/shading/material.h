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
        union {
            Texture *Albedo;  // NOTE(Joey): texture unit 0
            Texture *Diffuse; // NOTE(Joey): texture unit 0
        };
        union {
            Texture *MetalRoughness; // NOTE(Joey): metal: RGB channel; roughness: A channel
            Texture *Specular;       // NOTE(Joey): texture unit 1
        };
        Texture *Normal;    // NOTE(Joey): texture unit 2
        Texture *Occlusion; // NOTE(Joey): texture unit 3
        Texture *Emission;  // NOTE(Joey): texture unit 4

        math::vec4 Color = math::vec4(1.0f);

        // depth state
        bool   DepthTest    = true;
        bool   DepthWrite   = true;
        GLenum DepthCompare = GL_LESS;

        // stencil state

        // face culling state
        bool   Cull             = false;
        GLenum CullFace         = GL_BACK;
        GLenum CullWindingOrder = GL_CCW;

        // blending state
        bool   Blend           = false;
        GLenum BlendSrc      = GL_SRC_ALPHA;
        GLenum BlendDst      = GL_ONE_MINUS_SRC_ALPHA;
        GLenum BlendEquation = GL_FUNC_ADD;

    private:

    public:
    };
}
#endif