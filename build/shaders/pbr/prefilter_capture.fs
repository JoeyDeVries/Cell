#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

#include ../common/constants.glsl
#include ../common/sampling.glsl

uniform samplerCube environment;
uniform float roughness;

void main(void) {
    // the world vector acts as the normal of a tangent surface from the origin, 
    // aligned to vWorld. Given this normal, calculate all incoming radiance of 
    // the environment.
    vec3 N = normalize(WorldPos);
    // approximate view/reflection angle as equal to N (Unreal; split-sum)
    vec3 R = N;
    vec3 V = N;

    const uint SAMPLE_COUNT = 4096u * 4u;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    // for debugging, make scope visible to end part
    for(uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        // also try without NdotL!
        if(NdotL > 0.0)
        {
            // note that HDR environment map is loaded linearly, so no need for linearizing first.
            prefilteredColor += texture(environment, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;
    FragColor = vec4(prefilteredColor, 1.0);
}