#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

#include ../common/constants.glsl
#include ../common/sampling.glsl

uniform samplerCube environment;

void main()
{
	// NOTE(Joey): the world vector acts as the normal of a tangent surface
    // from the origin, aligned to WorldPos. Given this normal, calculate all
    // incoming radiance of the environment. The result of this radiance
    // is the radiance of light coming from -Normal direction, which is what
    // we use in the PBR shader to sample irradiance.
    vec3 N = normalize(WorldPos);

    // NOTE(Joey): tangent space calculation from origin point
    vec3 up    = vec3(0.0, 1.0, 0.0);
    vec3 right = cross(up, N);
    up         = cross(N, right);

    vec3 irradiance = vec3(0.0);
	int samples = 0;
	
	float sampleDelta = 0.025f;

    for(float phi = 0.0; phi < 2.0*PI; phi += sampleDelta)
    {
        for(float theta = 0.0; theta < 0.5*PI; theta += sampleDelta)
        {
            vec3 tangentSample = vec3(cos(phi) * sin(theta), sin(phi) * sin(theta), cos(theta));
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * N;

			// TODO(Joey): use HDRi map as input; from spherical to cube.
            // NOTE(Joey): no need to store linearized, as HDR map is already in linear space.
            irradiance += texture(environment, sampleVec).rgb * cos(theta) * sin(theta);
            // NOTE(Joey): here we don't divide by PI as this is implicit in the environment's lighting.
            samples++;
        }
    }
    irradiance = PI * irradiance * (1.0 / float(samples));

    FragColor = vec4(irradiance, 1.0);
}