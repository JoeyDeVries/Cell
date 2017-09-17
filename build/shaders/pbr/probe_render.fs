#version 330 core
out vec4 FragColor;

in vec3 LocalPos;
in vec3 WorldPos;

uniform samplerCube PrefilterMap;
uniform vec3 CamPos;

void main()
{
    vec3 N = normalize(LocalPos);
    vec3 V = normalize(CamPos - WorldPos);
    vec3 R = reflect(-V, N);
    vec3 color = textureLod(PrefilterMap, R, 0.0).rgb;
    FragColor = vec4(color, 1.0);
}