#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

void main()
{
    vec4 albedoAO = texture(gAlbedoAO, TexCoords);
    vec4 normalRoughness = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    
    const float ambientTerm = 0.1;
    // FragColor.rgb = albedoAO.rgb * albedoAO.a;
    FragColor.rgb = albedoAO.rgb * ambientTerm;
    // FragColor.rgb = vec3(0.05, 0.05, 0.05);
    FragColor.a = 1.0;
}