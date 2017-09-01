#version 330 core
layout (location = 0) out vec4 gPositionMetallic;
layout (location = 1) out vec4 gNormalRoughness;
layout (location = 2) out vec4 gAlbedoAO;

in vec2 UV0;
in vec3 FragPos;
in vec3 Normal;

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
    // also store the per-fragment normals into the gbuffer
    gNormalRoughness.rgb = normalize(Normal);
    gNormalRoughness.a = texture(TexRoughness, UV0).r;
    // and the diffuse per-fragment color
    gAlbedoAO.rgb = texture(TexAlbedo, UV0).rgb;
    gAlbedoAO.a = texture(TexAO, UV0).r;
}  