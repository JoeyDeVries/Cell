#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

#include ../common/constants.glsl
#include ../common/brdf.glsl

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

uniform sampler2D screenColor;
uniform sampler2D screenColorBlur;

uniform samplerCube envSpecular;
uniform sampler2D BRDFLUT;

uniform mat4 projection;
uniform mat4 view;

const float step = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth);
vec4 rayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth);

void main()
{
    vec4 normalRoughness  = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    vec4 albedoAO         = texture(gAlbedoAO, TexCoords);
    
    vec3 albedo     = albedoAO.rgb;
    vec3 viewPos    = positionMetallic.xyz;
    vec3 normal     = normalRoughness.rgb;
    float roughness = normalRoughness.a;
    float metallic  = positionMetallic.a;
    // roughness = 0.0;
       
    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(-viewPos); 
    vec3 R = reflect(-V, N);

    vec3 hitPos = viewPos;
    float dDepth;
 
    vec4 coords = rayMarch(R * max(minRayStep, -viewPos.z), hitPos, dDepth);
    vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - coords.xy));
    float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);

    vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);
    vec3 F   = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 SSRSmooth = texture(screenColor, coords.xy).rgb;
    vec3 SSRRough  = texture(screenColorBlur, coords.xy).rgb;   
    vec3 SSR = mix(SSRSmooth, SSRRough, roughness) * clamp(screenEdgefactor, 0.0, 0.9) * F;     
    
    float backfacingFactor = 1.0 - smoothstep(-0.5, 0.0, R.z);
    SSR *= backfacingFactor;
    
    float error = coords.z; // error as alpha, blend skylight based on error
    
    FragColor = vec4(SSR , error);
    // FragColor = vec4(SSRRough, error);
    // FragColor = vec4(texture(screenColorBlur, TexCoords).rgb, error);
}

vec3 binarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPositionMetallic, projectedCoord.xy).z;
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}

vec4 rayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    dir *= step;
 
    float error = 1.0;
    float depth;
    int steps;
    vec4 projectedCoord;
 
    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = projection * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        depth = texture(gPositionMetallic, projectedCoord.xy).z;
        if(depth > 1000.0)
            continue;
 
        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {   
                vec4 Result;
                Result = vec4(binarySearch(dir, hitCoord, dDepth), 1.0);
                error = 0.0;
                return Result;
            }
        }
        
        steps++;
    }
 
    return vec4(projectedCoord.xy, depth, error);
}