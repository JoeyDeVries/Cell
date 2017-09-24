#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV0;

out vec2 TexCoords;
out vec3 CamPos;

#include ../common/uniforms.glsl

void main()
{
	TexCoords = aUV0;
	gl_Position = vec4(aPos, 1.0);
    
    CamPos = vec3(view * vec4(camPos, 1.0));
}