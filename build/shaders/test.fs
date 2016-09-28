#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

#include test_include.glsl
#include common/constants.glsl
#include common/brdf.glsl
#include common/sampling.glsl

uniform float testUniform;
uniform sampler2D testTexture;

uniform vec3 PointLight0_Pos;
uniform vec3 PointLight0_Col;
uniform vec3 PointLight1_Pos;
uniform vec3 PointLight1_Col;

void main()
{
	vec3 N = normalize(Normal);
	vec3 L0 = normalize(PointLight0_Pos - WorldPos);
	vec3 L1 = normalize(PointLight1_Pos - WorldPos);
	
	float lambert = max(dot(N, L0), 0.0f);
	vec3 diffuse = texture(testTexture, TexCoords).rgb * lambert * PointLight0_Col;
	lambert = max(dot(N, L1), 0.0f);
	diffuse += texture(testTexture, TexCoords).rgb * lambert * PointLight1_Col;
	
	diffuse += vec3(0.005);// ambient
	
	FragColor = vec4(diffuse, 1.0);
	// FragColor = vec4(TexCoords, 0.0f, 1.0f);
	// FragColor = vec4(Normal, 1.0);
}
