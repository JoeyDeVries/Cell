// ----------------------------------------------------------------------------
// calculation without bit-operators. This might be required for certain
// glsl versions or ES/WebGL versions that don't support bit-operators yet.
// VanDerCorpus with base 2 equals hammersley's second value.
float VanDerCorpus(uint i, uint base)
{
	return 0.0;
}
// ----------------------------------------------------------------------------
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// efficient VanDerCorpus calculation.
float RadicalInverse_VdC(uint bits) 
{
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}
// ----------------------------------------------------------------------------
vec2 Hammersley(uint i, uint N)
{
	return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}
// ----------------------------------------------------------------------------
vec2 HammersleyNoBitOps(uint i, uint N)
{
	return vec2(float(i)/float(N), VanDerCorpus(i, 2));
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
vec3 SampleHemisphereUniform(float u, float v)
{
	float phi = v * 2.0 * PI;
	float cosTheta = sqrt(1.0 - u);
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
// ----------------------------------------------------------------------------
vec3 SampleHemisphereCos(float u, float v)
{
	float phi = v * 2.0 * PI;
	float cosTheta = 1.0 - u;
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
}
// ----------------------------------------------------------------------------
vec3 ImportanceSampleGGX(vec2 hammersley, vec3 N, float roughness)
{
	float a = roughness*roughness;
	
	float phi = 2.0 * PI * hammersley.x;
	float cosTheta = sqrt((1.0 - hammersley.y) / (1.0 + (a*a - 1.0) * hammersley.y));
	float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
	// NOTE(Joey): from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	// NOTE(Joey): from tangent-space H vector to world-space sample vector
	vec3 up          = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return sampleVec;
}
