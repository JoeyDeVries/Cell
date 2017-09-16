#ifndef BRDF_GLSL
#define BRDF_GLSL
// ----------------------------------------------------------------------------
// GGX/Throwbridge-Reitz
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float nom    = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return nom / denom;
}

// ----------------------------------------------------------------------------
// TODO(Joey): get proper names of the options/functions
float GeometryGGXSchlick(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom    = NdotV;
	float denom = NdotV * (1.0 - k) + k ;
	
	return nom / denom;
}
// ----------------------------------------------------------------------------
// NOTE(Joey): for IBL we use a different k (see unreal course notes)
float GeometryGGXSchlickIBL(float NdotV, float roughness)
{
	float a = roughness*roughness;
	float k = a / 2.0;
		
	float nom    = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometryGGX(float NdotV, float NdotL, float roughness)
{
	float ggx2 = GeometryGGXSchlick(NdotV, roughness);
	float ggx1 = GeometryGGXSchlick(NdotL, roughness);
	
	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float GeometryGGXIBL(float NdotV, float NdotL, float roughness)
{
	float ggx1 = GeometryGGXSchlickIBL(NdotV, roughness);
	float ggx2 = GeometryGGXSchlickIBL(NdotL, roughness);
	
	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
	return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 FresnelSphericalGaussian(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * cosTheta - 6.98316 * cosTheta));
}
#endif