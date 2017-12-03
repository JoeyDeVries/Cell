#version 430 core
out vec4 FragColor;

in vec3 FragPos;
in vec4 ScreenPos;
in vec3 CamPos;
in vec3 ProbePos;

#include ../common/constants.glsl
#include ../common/brdf.glsl
#include ../common/uniforms.glsl

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

uniform samplerCube envIrradiance;
uniform samplerCube envPrefilter;
uniform sampler2D   BRDFLUT;
uniform sampler2D   TexSSAO;

uniform vec3 probePos;
uniform float probeRadius;

uniform int SSAO;
uniform int SSR;

void main()
{
    vec2 uv = (ScreenPos.xy / ScreenPos.w) * 0.5 + 0.5;
    
    vec4 albedoAO         = texture(gAlbedoAO, uv);
    vec4 normalRoughness  = texture(gNormalRoughness, uv);
    vec4 positionMetallic = texture(gPositionMetallic, uv);
    float ao = 1.0;
    if(SSAO)
    {
        ao = texture(TexSSAO, uv).r;
    }
    
    vec3 worldPos   = positionMetallic.xyz;
    vec3 albedo     = albedoAO.rgb;
    vec3 normal     = normalRoughness.rgb;
    float roughness = normalRoughness.a;
    float metallic  = positionMetallic.a;
    
       
    // lighting input
    vec3 N = normalize(normal);
    vec3 V = normalize(camPos.xyz - worldPos); 
    vec3 L = normalize(probePos - worldPos);
    vec3 H = normalize(V + L);     
    vec3 R = reflect(-V, N); 
    
    // calculate light radiance    
    float attenuation = pow(max(1.0 - length(worldPos - probePos) / probeRadius, 0.0), 2.0);
        
    // cook-torrance brdf
    vec3 F0 = vec3(0.04); // base reflectance at incident angle for non-metallic (dia-conductor) surfaces 
	F0 = mix(F0, albedo, metallic);
	vec3 F   = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
	
	// calculate specular global illumination contribution w/ Epic's split-sum approximation
    vec3 specular = vec3(0.0);
    if(SSR == 0)
    {
        const float MAX_REFLECTION_LOD = 5.0;
        vec3 prefilteredColor = textureLod(envPrefilter, R,  roughness * MAX_REFLECTION_LOD).rgb;
        vec2 envBRDF          = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        specular = prefilteredColor * (F * envBRDF.x + envBRDF.y);
    }
    
	// for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light) so to preserve this
    // relationship the diffuse component (kD) equals 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// also multiply kD by the inverse metalness s.t. only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals have
    // no diffuse light).
	kD *= 1.0 - metallic;	
	// directly obtain irradiance from irradiance environment map
	vec3 irradiance = texture(envIrradiance, N).rgb;
	vec3 diffuse = albedo * irradiance;
	
	// combine contributions, note that we don't multiply by kS as kS equals
    // the Fresnel value and the specular value was already multipled by Fresnel
    // during the importance sampling.
	vec3 color = (kD * diffuse + specular * 0.4) * ao * attenuation; 
    
    FragColor.rgb = color;
    FragColor.a = 1.0;
}