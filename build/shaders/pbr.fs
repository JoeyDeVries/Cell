#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

#include common/constants.glsl
#include common/brdf.glsl
// #include common/sampling.glsl

uniform samplerCube EnvIrradiance;
uniform samplerCube EnvPrefilter;
uniform sampler2D    BRDFLUT;


uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

uniform vec3 PointLight0_Pos;
uniform vec3 PointLight0_Col;
uniform vec3 PointLight1_Pos;
uniform vec3 PointLight1_Col;

uniform vec3 CamPos;
uniform float Exposure;

vec3 getNormal(vec3 worldNormal, vec3 tangentNormal)
{
    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 normal   = normalize(worldNormal);
    vec3 tangent  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 binormal = -normalize(cross(normal, tangent));
    mat3 TBN      = mat3(tangent, binormal, normal);

    return normalize(TBN * tangentNormal);
}


void main()
{
	// NOTE(Joey): retrieve PBR material parameters
#ifndef ALPHA
	vec3 albedo = pow(texture(TexAlbedo, TexCoords).rgb, vec3(2.2));
#else 
    vec4 sampledAlbedo = texture(TexAlbedo, TexCoords);
    vec3 albedo = pow(sampledAlbedo.rgb, vec3(2.2));
    float alpha = sampledAlbedo.a;
#endif
	vec3 tNormal = texture(TexNormal, TexCoords).xyz * 2.0 - 1.0;
	float metallic = texture(TexMetallic, TexCoords).r;
	float roughness = texture(TexRoughness, TexCoords).r;
	float ao 		= texture(TexAO, TexCoords).r;
		
	// NOTE(Joey): pre-calculate lighting data
	vec3 N = getNormal(Normal, tNormal);
	vec3 V = normalize(CamPos - WorldPos);
	vec3 R = reflect(-V, N); 
	
	// NOTE(Joey): calculate color/reflectance at normal incidence
    // NOTE(Joey): if dia-electric (like plastic) use F0 as 0.04 and
    // if it's a metal, use their albedo color as F0 (metallic workflow)
	 // NOTE(Joey): kS is equal to Fresnel
	// NOTE(Joey): calculate reflectance w/ (modified for roughness) Fresnel
	vec3 F0 = vec3(0.04); // NOTE(Joey): base reflectance at incident angle for non-metallic (dia-conductor) surfaces 
	F0 = mix(F0, albedo, metallic);
	// vec3 F   = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 F   = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
	
	// NOTE(Joey): calculate specular global illumination contribution w/ Epic's split-sum approximation
	const float MAX_REFLECTION_LOD = 5.0;
    vec3 prefilteredColor = pow(textureLod(EnvPrefilter, R,  roughness * MAX_REFLECTION_LOD).rgb, vec3(2.2));
    vec2 envBRDF          = texture(BRDFLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular           = prefilteredColor * (F * envBRDF.x + envBRDF.y);

	 // NOTE(Joey): for energy conservation, the diffuse and specular light can't
    // be above 1.0 (unless the surface emits light) so to preserve this
    // relationship the diffuse component (kD) equals 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// NOTE(Joey): also multiply kD by the inverse metalness s.t. only non-metals
    // have diffuse lighting, or a linear blend if partly metal (pure metals have
    // no diffuse light).
	kD *= 1.0 - metallic;	
	// NOTE(Joey): directly obtain irradiance from irradiance environment map
	// vec3 irradiance = pow(texture(EnvIrradiance, N).rgb, vec3(2.2));
	vec3 irradiance = texture(EnvIrradiance, N).rgb;
	vec3 diffuse = albedo * irradiance;
	
	 // NOTE(Joey): combine contributions, note that we don't multiply by kS as kS equals
    // the Fresnel value and the specular value was already multipled by Fresnel
    // during the importance sampling.
	vec3 color = kD * diffuse + specular; 
	
	// NOTE(Joey): as the IBL lighting of both diffuse and specular counts as
    // (GI) ambient lighting, we multiply both with the AO component.
    color *= ao;
	
	// NOTE(Joey): HDR tonemapping
	color = vec3(1.0) - exp(-color * Exposure);
	
	// NOTE(Joey): gamma correct
	color = pow(color, vec3(1.0/2.2)); 
    
    #ifdef COLOR_SQUARE
        color = color * color;
    #endif
    
    #ifdef COLOR_RED
        color = color * vec3(1.0, 0.0, 0.0);
    #endif
    
    #ifdef TEST
        color += vec3(0.0, 1.0, 0.0);
    #endif
	
#ifndef ALPHA
	FragColor = vec4(color, 1.0);
#else
    FragColor = vec4(color, alpha);
#endif
    // FragColor = vec4(vec3(metallic), 1.0);
    // FragColor = vec4(TexCoords, 0.0, 1.0);
}
