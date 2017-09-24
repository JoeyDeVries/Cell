#version 330 core
layout (location = 0) in vec3 pos;

#include common/uniforms.glsl

uniform mat4 model;

void main()
{
	gl_Position =  projection * view * model * vec4(pos, 1.0);
}