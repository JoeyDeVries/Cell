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
    vec3 dirLight0_Dir;
    vec3 dirLight0_Col;
    vec3 dirLight1_Dir;
    vec3 dirLight1_Col;
    vec3 dirLight2_Dir;
    vec3 dirLight2_Col;
    vec3 dirLight3_Dir;
    vec3 dirLight4_Col;
    vec3 pointLight0_Pos;
    vec3 pointLight0_Col;
    vec3 pointLight1_Pos;
    vec3 pointLight1_Col;
    vec3 pointLight2_Pos;
    vec3 pointLight2_Col;
    vec3 pointLight3_Pos;
    vec3 pointLight3_Col;
    vec3 pointLight4_Pos;
    vec3 pointLight4_Col;
    vec3 pointLight5_Pos;
    vec3 pointLight5_Col;
    vec3 pointLight6_Pos;
    vec3 pointLight6_Col;
    vec3 pointLight7_Pos;
    vec3 pointLight7_Col;
};
#endif