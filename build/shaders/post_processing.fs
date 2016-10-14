#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{
    // TODO(Joey): do gamma correction/HDR tonemapping here
    FragColor = texture(TexSrc, TexCoords);
}