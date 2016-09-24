#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube background;

void main()
{
	vec3 color = texture(background, WorldPos).rgb;

	FragColor = vec4(color, 1.0);
}