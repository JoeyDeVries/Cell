#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube background;

uniform float lodLevel;
uniform float Exposure;

void main()
{
	vec3 color = textureLod(background, WorldPos, lodLevel).rgb;
	FragColor = vec4(color, 1.0);
}