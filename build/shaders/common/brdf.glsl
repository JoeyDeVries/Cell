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
float GeometryGGXSchlick(vec3 N, vec3 V, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;
	
	float NdotV = max(dot(N, V), 0.0);
	
	float nom    = NdotV;
	float denom = NdotV * (1.0 - k) + k ;
	
	return nom / denom;
}
// ----------------------------------------------------------------------------
// NOTE(Joey): for IBL we use a different k (see unreal course notes)
float GeometryGGXSchlickIBL(vec3 N, vec3 V, float roughness)
{
	float k = roughness / 2.0;
	
	float NdotV = max(dot(N, V), 0.0);
	
	float nom    = NdotV;
	float denom = NdotV * (1.0 - k) + k ;
	
	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometryGGX(vec3 N, vec3 L, vec3 V, float roughness)
{
	float ggx1 = GeometryGGXSchlick(N, L, roughness);
	float ggx2 = GeometryGGXSchlick(N, V, roughness);
	
	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
float GeometryGGXIBL(vec3 N, vec3 L, vec3 V, float roughness)
{
	float ggx1 = GeometryGGXSchlickIBL(N, L, roughness);
	float ggx2 = GeometryGGXSchlickIBL(N, V, roughness);
	
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
	return F0 + (1.0 - F0 * roughness) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
vec3 FresnelSphericalGaussian(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * cosTheta - 6.98316 * cosTheta));
}