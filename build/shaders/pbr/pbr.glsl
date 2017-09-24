#ifndef PBR_GLSL
#define PBR_GLSL

#include ../common/constants.glsl
#include ../common/brdf.glsl

vec3 PBRAnalyticLighting(
    vec3 albedo,
    vec3 normal,
    float metallic,
    float roughness,
    vec3 fragPos,
    vec3 viewPos, 
    vec4 lightVec,
    vec3 lightColor,
    float lightRadius
)
{
    // input
    vec3 N = normalize(normal);
    vec3 V = normalize(-viewPos); // view-space camera is (0, 0, 0): (0, 0, 0) - viewPos = -viewPos
    vec3 L = vec3(0.0);
    if(lightVec.w > 0.5) // light is a positional light (point/spot)
    {   
        L = normalize(lightVec.xyz - viewPos);
    }
    else // light is a directional light (has no position)
    {   
        L = normalize(-lightVec.xyz);
    }
    vec3 H = normalize(V + L);     
    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);
    
    // calculate light radiance   
    float attenuation = 1.0;
    if(lightRadius != 0.0)
    {
        attenuation = max(0.95 - length(viewPos - lightVec.xyz) / lightRadius, 0.0);
    }
    vec3 radiance = lightColor * attenuation;   
    
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
    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL; 
    
    return Lo;
}
#endif