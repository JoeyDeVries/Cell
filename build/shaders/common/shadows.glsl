#ifndef SHADOW_GLSL
#define SHADOW_GLSL
float ShadowFactor(sampler2D shadowMap, vec4 fragPosLightSpace, vec3 N, vec3 L)
{   
    // perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // shadow bias
    float bias = max(0.05 * (1.0 - dot(N, L)), 0.005);  
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;    
}
#endif