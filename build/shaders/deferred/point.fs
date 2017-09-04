#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPositionMetallic;
uniform sampler2D gNormalRoughness;
uniform sampler2D gAlbedoAO;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float lightRadius;

uniform vec3 CamPos;

void main()
{
    vec4 albedoAO = texture(gAlbedoAO, TexCoords);
    vec4 normalRoughness = texture(gNormalRoughness, TexCoords);
    vec4 positionMetallic = texture(gPositionMetallic, TexCoords);
    
    vec3 worldPos = positionMetallic.xyz;
    
    vec3 N = normalize(normalRoughness.xyz);
    vec3 L = normalize(lightPos - worldPos);
    vec3 V = normalize(CamPos - worldPos);
    vec3 H = normalize(L + V);

    float lambert = max(dot(L, N), 0.0);    
    float specular = pow(max(dot(H, N), 0.0), 16.0);      
    
    float attenuation = 1.0 - length(worldPos - lightPos) / lightRadius;
    
    vec3 color = lambert * albedoAO.rgb + specular * lightColor;
    color *= attenuation;
    
    FragColor.rgb = color;
    FragColor.a = 1.0;
}