#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

uniform vec3 lightDir;
uniform vec3 lightColor;

uniform vec3 CamPos;

void main()
{
    vec4 albedoAO = texture(gAlbedoAO, TexCoords);
    vec4 normalRoughness = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    
    vec3 worldPos = positionMetallic.xyz;
    
    vec3 N = normalize(normalRoughness.xyz);        
    vec3 V = normalize(CamPos - worldPos);
    vec3 H = normalize(-lightDir + V);
    
    float lambert = max(dot(-lightDir, N), 0.0);
    float specular = pow(max(dot(H, N), 0.0), 16.0);
    
    vec3 color = lambert * lightColor * albedoAO.rgb + specular * lightColor;
    
    FragColor.rgb = color;
    FragColor.a = 1.0;
}