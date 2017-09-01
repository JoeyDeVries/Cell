#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

#include ../common/constants.glsl
#include ../common/sampling.glsl

uniform sampler2D environment;

void main()
{
	vec2 uv = SampleSphericalMap(normalize(WorldPos));
	vec3 color = texture(environment, uv).rgb;
    FragColor = vec4(color, 1.0);
}