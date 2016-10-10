#version 330 core
out vec4 FragColor;

in vec3 WorldPos;

uniform samplerCube background;

uniform float lodLevel;
uniform float Exposure;

void main()
{
	vec3 color = textureLod(background, WorldPos, lodLevel).rgb;
	// vec3 color = texture(background, WorldPos).rgb;
	
	// NOTE(Joey): HDR tonemapping
	color = vec3(1.0) - exp(-color * Exposure);
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);
}