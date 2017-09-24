#version 330 core
out vec4 FragColor;
in vec3 LocalPos;

uniform samplerCube background;

// uniform float lodLevel;

void main()
{
	vec3 color = textureLod(background, LocalPos, 1.0).rgb;
	FragColor = vec4(color, 1.0);
}