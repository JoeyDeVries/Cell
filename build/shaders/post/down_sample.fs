#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{             
    vec4 color = texture(TexSrc, TexCoords);
    FragColor = color;
}