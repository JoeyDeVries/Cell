#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{
    FragColor.rgb = texture(TexSrc, TexCoords).rgb;
    FragColor.rgb *= vec3(1.0, 0.8, 0.8);
    FragColor.a = 1.0;
}