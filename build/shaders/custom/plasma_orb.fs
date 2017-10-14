#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

#include ../common/uniforms.glsl

uniform vec3 PlasmaColor;

void main()
{
    // get normal
    vec3 dx = normalize(dFdx(FragPos));
    vec3 dy = normalize(dFdy(FragPos));
    vec3 N  = cross(dx, dy);

    // rim lighting
    vec3 V = normalize(camPos.xyz - FragPos);
    float rim = 1.0 - abs(dot(N, V));
    rim = pow(rim, 4.0);
    
    const vec3 rimColor = vec3(0.001, 0.0015, 1.0) * 1224.9;
    
    FragColor = vec4(rim * rimColor, 1.0);
}
