#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

#include common/uniforms.glsl

uniform mat4 model;

void main()
{
	TexCoords = texCoords;
	FragPos   = vec3(model * vec4(pos, 1.0));
	Normal    = mat3(model) * normal;
    
	gl_Position =  projection * view * vec4(FragPos, 1.0);
}