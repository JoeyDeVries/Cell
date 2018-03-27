#version 420 core
layout (location = 0) in vec3 aPos;

#include common/uniforms.glsl

// uniform mat4 projection;
// uniform mat4 view;

out vec3 WorldPos;

void main()
{
    WorldPos = aPos;

	mat4 rotView = mat4(mat3(view));
	vec4 clipPos = projection * rotView * vec4(WorldPos, 1.0);

	gl_Position = clipPos.xyww;
}