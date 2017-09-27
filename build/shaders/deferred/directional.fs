#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/shadows.glsl
#include ../common/uniforms.glsl

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform sampler2D lightShadowMap;
uniform mat4 lightShadowViewProjection;

void main()
{
    vec4 albedoAO = texture(gAlbedoAO, TexCoords);
    vec4 normalRoughness = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    
    vec3 worldPos   = positionMetallic.xyz;
    vec3 albedo     = albedoAO.rgb;
    vec3 normal     = normalRoughness.rgb;
    float roughness = normalRoughness.a;
    float metallic  = positionMetallic.a;
       
    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos.xyz - worldPos); // view-space camera is (0, 0, 0): (0, 0, 0) - viewPos = -viewPos
    vec3 L = normalize(-lightDir);
    vec3 H = normalize(V + L);     
	              
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
          
    // calculate light radiance    
    vec3 radiance = lightColor;        
    
    // light shadow
    vec4 fragPosLightSpace = lightShadowViewProjection * vec4(worldPos, 1.0);
    float shadow = ShadowFactor(lightShadowMap, fragPosLightSpace, N, L);
    
    // cook-torrance brdf
    float NDF = DistributionGGX(N, H, roughness);        
    float G   = GeometryGGX(max(dot(N, V), 0.0), max(dot(N, L), 0.0), roughness);      
    vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 nominator    = NDF * G * F;
    float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; 
    vec3 specular     = nominator / denominator;
        
    // add to outgoing radiance Lo
    float NdotL = max(dot(N, L), 0.0);                
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL * (1.0 - shadow); 
        
    FragColor.rgb = Lo;
    // FragColor.rgb = vec3(shadow);
    FragColor.a = 1.0;
}