#ifndef UNIFORMS_H
#define UNIFORMS_H
// global uniform buffer for shared common set of uniforms among programs
// see: https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL for table of std140 byte offsets
layout (std140, binding = 0) uniform Global
{
    // trtansformations
    mat4 viewProjection;
    mat4 projection;
    mat4 view;
    mat4 invView;
    // scene
    vec3 camPos;
    // lighting
};
#endif