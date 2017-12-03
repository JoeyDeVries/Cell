#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D texNoise;

uniform vec2 renderSize;
uniform vec3 kernel[64];
uniform int sampleCount;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    float radius = 0.5;
    float bias = 0.025;
    vec2 noiseScale = renderSize.xy * vec2(1.0 / 4.0);
    
    vec4 normalRoughness  = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    vec3 randomVec        = texture(texNoise, TexCoords * noiseScale).xyz;
    
    vec3 fragPos = (view * vec4(positionMetallic.xyz, 1.0)).xyz;
    vec3 normal  = mat3(view) * normalRoughness.xyz;
    
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);  
    
    float occlusion = 0.0;
    for(int i = 0; i < sampleCount; ++i)
    {
        // get sample position
        vec3 sample = TBN * kernel[i]; // from tangent to view-space
        sample = fragPos + sample * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(sample, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = (view * vec4(texture(gPositionMetallic, offset.xy).xyz, 1.0)).z;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= sample.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / float(sampleCount));
    occlusion = pow(occlusion, 3.0);

    FragColor = vec4(vec3(occlusion), 1.0);
}