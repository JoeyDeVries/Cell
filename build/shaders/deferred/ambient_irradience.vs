#version 430 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec4 ScreenPos;
out vec3 CamPos;
out vec3 ProbePos;

#include ../common/uniforms.glsl

uniform mat4 model;

uniform vec3 probePos;

void main()
{
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    ScreenPos = projection * vec4(FragPos, 1.0);
    
    CamPos = vec3(view * vec4(camPos.xyz, 1.0));
    ProbePos = vec3(view * vec4(probePos, 1.0));
    
	gl_Position = ScreenPos;
}