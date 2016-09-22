#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

#include test_include.glsl

uniform float testUniform;
uniform sampler2D testTexture;

uniform vec3 PointLight0_Pos;
uniform vec3 PointLight0_Col;

void main()
{
	vec3 N = normalize(Normal);
	vec3 L = normalize(PointLight0_Pos - WorldPos);
	
	float lambert = max(dot(N, L), 0.0);
	vec3 diffuse = texture(testTexture, TexCoords).rgb * lambert * PointLight0_Col;
	
	FragColor = vec4(diffuse, 1.0);
	// FragColor = vec4(TexCoords, 0.0f, 1.0f);
	// FragColor = vec4(Normal, 1.0);
}
