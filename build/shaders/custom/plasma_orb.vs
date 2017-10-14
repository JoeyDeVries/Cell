#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

#include ../common/uniforms.glsl

uniform mat4 model;

uniform sampler2D TexPerllin;

uniform float Time;
uniform float Strength;
uniform float Speed;

void main()
{
    // sample perlin noise vector (build up as fractal sum: fractional brownian motion: each layer twice the frequency, half the amplitude)
    float offset  = Time * Speed;
    vec2 uv1     = (aUV + offset * 0.63);
    vec2 uv2     = (aUV + offset * 0.37) * 2.0;
    vec2 uv3     = (aUV + offset * 1.17) * 4.0;
    vec3 noise1 = (normalize(texture(TexPerllin, uv1).rgb * 2.0 - 1.0));
    vec3 noise2 = (normalize(texture(TexPerllin, uv2).rgb * 2.0 - 1.0)) * 0.5;
    vec3 noise3 = (normalize(texture(TexPerllin, uv3).rgb * 2.0 - 1.0)) * 0.25; 
    vec3 noise  = (noise1 + noise2 + noise3) / 1.75;
    vec3 localPos = aPos + noise * Strength;
    
	TexCoords = aUV;
	FragPos   = vec3(model * vec4(localPos, 1.0));
	Normal    = mat3(model) * aNormal;
    
	gl_Position =  projection * view * vec4(FragPos, 1.0);
}