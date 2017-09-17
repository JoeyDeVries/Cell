#version 330 core
out vec4 FragColor;

in vec2 UV0;
in vec3 FragPos;

uniform sampler2D TexAlbedo;
// uniform sampler2D TexNormal;
// uniform sampler2D TexMetallic;
// uniform sampler2D TexRoughness;
// uniform sampler2D TexAO;

void main()
{    
    vec4 color = texture(TexAlbedo, UV0);
    // if(color.a <= 0.5)
    // {
        // discard;
    // }
    FragColor = vec4(color.rgb, 1.0);
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}  