#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

uniform bool horizontal;
uniform float range;

const float weights[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main()
{             
    vec2 texSize = textureSize(TexSrc, 0); 
    // vec2 texSize = vec2(247.32, 271.91);
    vec2 tex_offset = (1.0 / texSize) * range; // gets size of single texel
    vec3 result = texture(TexSrc, TexCoords).rgb * weights[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(TexSrc, TexCoords + vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
            result += texture(TexSrc, TexCoords - vec2(tex_offset.x * i, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for(int i = 1; i < 5; ++i)
        {
            result += texture(TexSrc, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[i];
            result += texture(TexSrc, TexCoords - vec2(0.0, tex_offset.y * i)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
}