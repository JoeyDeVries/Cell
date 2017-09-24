#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

in vec3 CamPosView;
in vec3 DirLightDirView;

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;

uniform vec3 DirLight0_Col;

#include pbr/pbr.glsl

void main()
{
    vec3 albedo = texture(TexAlbedo, TexCoords).rgb;
    vec3 normal = normalize(Normal); 
    float metallic = texture(TexMetallic, TexCoords).r;
    float roughness = texture(TexRoughness, TexCoords).r;
    
    vec3 color = PBRAnalyticLighting(
        albedo, normal, metallic, roughness, CamPosView,
        FragPos, vec4(DirLightDirView, 0.0), DirLight0_Col, 0.0
    );
    
    FragColor = vec4(albedo * 0.4 + color * 0.6, 1.0);
}
