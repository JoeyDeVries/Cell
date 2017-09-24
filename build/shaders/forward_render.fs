#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

in vec3 CamPosView;
in vec3 DirLightDirView;

#include common/uniforms.glsl

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

uniform vec3 DirLight0_Col;

#include pbr/pbr.glsl

void main()
{
    vec4 albedo = texture(TexAlbedo, TexCoords);
    vec3 normal = normalize(Normal); // TODO: normal mapping
    float metallic = texture(TexMetallic, TexCoords).r;
    float roughness = texture(TexRoughness, TexCoords).r;
    
    vec3 color = PBRAnalyticLighting(
        albedo.rgb, normal, metallic, roughness, CamPosView,
        FragPos, vec4(DirLightDirView, 0.0), DirLight0_Col, 0.0
    );
                      
    #ifdef ALPHA_DISCARD
        if(albedo.a <= 0.5) 
        {
            discard;
        }
        FragColor = vec4(color, 1.0);
    #elif defined(ALPHA_BLEND)
        FragColor = vec4(color, albedo.a);
    #else 
        FragColor = vec4(color, 1.0);
    #endif
}
