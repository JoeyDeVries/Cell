#version 330 core
out vec4 FragColor;

in vec3 color;
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#include common/shadows.glsl
#include common/uniforms.glsl
#include pbr/pbr.glsl

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

uniform sampler2D lightShadowMap1;
uniform mat4 lightShadowViewProjection1;

void main()
{
    vec4 albedo = texture(TexAlbedo, TexCoords);
    #ifdef ALPHA_BLEND
        albedo.rgb *= albedo.a; // pre-multiplied alpha
    #endif
    float metallic = texture(TexMetallic, TexCoords).r;
    float roughness = texture(TexRoughness, TexCoords).r;
    vec3 N = normalize(Normal); // TODO: normal mapping
    vec3 L = normalize(-dirLight0_Dir.xyz);
    
    vec3 color = PBRAnalyticLighting(
        albedo.rgb, N, metallic, roughness, camPos.xyz,
        FragPos, vec4(dirLight0_Dir.xyz, 0.0), dirLight0_Col.rgb, 0.0
    );
    vec4 fragPosLightSpace = lightShadowViewProjection1 * vec4(FragPos, 1.0);
    float shadow = ShadowFactor(lightShadowMap1, fragPosLightSpace, N, L);
    color.rgb *= max(1.0 - shadow, 0.1);
                      
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
