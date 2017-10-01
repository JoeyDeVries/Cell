#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#include common/uniforms.glsl

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;

#include pbr/pbr.glsl

void main()
{
    vec3 albedo = texture(TexAlbedo, TexCoords).rgb;
    vec3 normal = normalize(Normal); 
    float metallic = texture(TexMetallic, TexCoords).r;
    float roughness = texture(TexRoughness, TexCoords).r;
    
    vec3 color = PBRAnalyticLighting(
        albedo, normal, metallic, roughness, camPos.xyz,
        FragPos, vec4(dirLight0_Dir.xyz, 0.0), dirLight0_Col.rgb, 0.0
    );
    
    // FragColor = vec4(albedo * 0.4 + color * 0.6, 1.0);
    FragColor = vec4(albedo * 0.4 + color * 0.6, 1.0);
}
