#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec4 ScreenPos;
out vec3 CamPos;
out vec3 LightPos;

#include ../common/uniforms.glsl

uniform mat4 model;

uniform vec3 lightPos;

void main()
{
    FragPos = vec3(view * model * vec4(aPos, 1.0));
    ScreenPos = projection * vec4(FragPos, 1.0);
    
    CamPos = vec3(view * vec4(camPos, 1.0));
    LightPos = vec3(view * vec4(lightPos, 1.0));
    
	gl_Position = ScreenPos;
}