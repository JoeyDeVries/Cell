#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{
    FragColor.rgb = texture(TexSrc, TexCoords).rgb;
    FragColor.a = 1.0;
}