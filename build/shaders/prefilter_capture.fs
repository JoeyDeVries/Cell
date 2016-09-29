#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube environment;

void main()
{
	vec3 color = texture(environment, WorldPos).rgb;
	color *= vec3(0.0, 0.0, 1.0);

	FragColor = vec4(color, 1.0);
}