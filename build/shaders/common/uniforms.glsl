#ifndef UNIFORMS_H
#define UNIFORMS_H
// global uniform buffer for shared common set of uniforms among programs
// see: https://learnopengl.com/#!Advanced-OpenGL/Advanced-GLSL for table of std140 byte offsets
layout (std140, binding = 0) uniform Global
{
    // trtansformations
    mat4 viewProjection;
    mat4 prevViewProjection;
    mat4 projection;
    mat4 view;
    mat4 invViewz;
    // scene
    vec4 camPos;
    // lighting
    vec4 dirLight0_Dir;
    vec4 dirLight0_Col;
    vec4 dirLight1_Dir;
    vec4 dirLight1_Col;
    vec4 dirLight2_Dir;
    vec4 dirLight2_Col;
    vec4 dirLight3_Dir;
    vec4 dirLight4_Col;
    vec4 pointLight0_Pos;
    vec4 pointLight0_Col;
    vec4 pointLight1_Pos;
    vec4 pointLight1_Col;
    vec4 pointLight2_Pos;
    vec4 pointLight2_Col;
    vec4 pointLight3_Pos;
    vec4 pointLight3_Col;
    vec4 pointLight4_Pos;
    vec4 pointLight4_Col;
    vec4 pointLight5_Pos;
    vec4 pointLight5_Col;
    vec4 pointLight6_Pos;
    vec4 pointLight6_Col;
    vec4 pointLight7_Pos;
    vec4 pointLight7_Col;
};
#endif