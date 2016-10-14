#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{
    FragColor = vec4(texture(TexSrc, TexCoords).rgb * vec3(0.25, 0.25, 1.0), 1.0);
}