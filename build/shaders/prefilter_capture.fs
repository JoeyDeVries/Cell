#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

#include common/constants.glsl
#include common/sampling.glsl

uniform samplerCube environment;
uniform float roughness;

void main(void) {
    // NOTE(Joey): the world vector acts as the normal of a tangent surface
    // from the origin, aligned to vWorld. Given this normal, calculate all
    // incoming radiance of the environment.
    vec3 N = normalize(WorldPos);
    // NOTE(Joey): approximate view/reflection angle as equal to N (Unreal; split-sum)
    vec3 R = N;
    vec3 V = N;

    const int SAMPLE_COUNT = 4096;
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;

    // NOTE(Joey): for debugging, make scope visible to end part
    for(int i = 0; i < SAMPLE_COUNT; ++i)
    {
        // NOTE(Joey): generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = ImportanceSampleGGX(Xi, N, roughness);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(dot(N, L), 0.0);
        // NOTE(Joey): also try without NdotL!
        if(NdotL > 0.0)
        {
            // NOTE(Joey): store linearly, see discussion at irradiance.fs.glsl (and images at
            // data folder of modelPreviewer).
            // NOTE(Joey): note that HDR environment map is loaded linearly, so no need for linearizing first.
            // NOTE(Joey): write down that with high-contrast HDR images (like grace-new.hdr) you need A LOT
            // more samples for a proper integration (otherwise result resembles a disco light party); so for
            // pre-computing this at start-up be sure to use more neutral HDR maps (like eagle_hill).
//            prefilteredColor += pow(textureCube(_CubeSampler, L).rgb, vec3(2.2)) * NdotL;
            prefilteredColor += texture(environment, L).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    prefilteredColor = prefilteredColor / totalWeight;

    FragColor = vec4(prefilteredColor, 1.0);
}