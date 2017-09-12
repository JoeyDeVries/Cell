#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D HDRScene;

void main()
{
    vec3 color = texture(HDRScene, TexCoords).rgb;
    float grayscale = dot(color, vec3(0.299, 0.587, 0.114));
    color = smoothstep(1.0, 2.0, grayscale) * color;
    
    FragColor.rgb = color;
    FragColor.a = 1.0;
}