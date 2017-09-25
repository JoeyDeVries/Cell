#version 330 core
layout (location = 0) out vec4 gPositionMetallic;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoAO;
layout (location = 3) out vec4 gMotion; // TODO: for ease of use with development; package g-buffer properties (linear depth instead of position, normal as xz pair etc.)

in vec2 UV0;
in vec3 FragPos;
in mat3 TBN;
in vec4 ClipSpacePos;
in vec4 PrevClipSpacePos;

uniform sampler2D TexAlbedo;
uniform sampler2D TexNormal;
uniform sampler2D TexMetallic;
uniform sampler2D TexRoughness;
uniform sampler2D TexAO;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPositionMetallic.rgb = FragPos;
    gPositionMetallic.a = texture(TexMetallic, UV0).r;
    // also store the per-fragment (bump-)normals into the gbuffer
    float roughness = texture(TexRoughness, UV0).r;
    vec3 N = texture(TexNormal, UV0).rgb;    
    N = normalize(N * 2.0 - 1.0);
    // N = mix(N, vec3(0.0, 0.0, 1.0), pow(roughness, 0.5)); // smooth normal based on roughness (to reduce specular aliasing)
    // N.x *= 2.0;
    // N.y *= 2.0;
    N = normalize(TBN * N);  
    // N = TBN[2];
    gNormalRoughness.rgb = normalize(N);
    gNormalRoughness.a = roughness;
    // and the diffuse per-fragment color
    gAlbedoAO.rgb = texture(TexAlbedo, UV0).rgb;
    gAlbedoAO.a = texture(TexAO, UV0).r;
    // per-fragment motion vector
    vec2 clipSpace = ClipSpacePos.xy / ClipSpacePos.w;
    vec2 prevClipSpace = PrevClipSpacePos.xy / PrevClipSpacePos.w;
    gMotion.xy = clipSpace - prevClipSpace;
}  