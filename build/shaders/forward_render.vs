#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

out vec3 CamPosView;
out vec3 DirLightDirView;

#include common/uniforms.glsl

uniform mat4 model;

uniform vec3 DirLight0_Dir;

void main()
{
	TexCoords = texCoords;
	FragPos   = vec3(view * model * vec4(pos, 1.0f));
	Normal    = mat3(view * model) * normal;
    
    CamPosView      = vec3(view * vec4(camPos, 1.0));
    DirLightDirView = vec3(view * vec4(DirLight0_Dir, 0.0));
	
	gl_Position =  projection * vec4(FragPos, 1.0);
}