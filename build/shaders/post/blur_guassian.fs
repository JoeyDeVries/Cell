#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

uniform bool horizontal;

const float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
const float weights[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );

void main()
{             
    vec2 texSize = textureSize(TexSrc, 0); 
    vec2 tex_offset = (1.0 / texSize); // gets size of single texel
    vec3 result = texture(TexSrc, TexCoords).rgb * weights[0]; // current fragment's contribution
    if(horizontal)
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(TexSrc, TexCoords + vec2(offset[i] / texSize.x, 0.0)).rgb * weights[i];
            result += texture(TexSrc, TexCoords - vec2(offset[i] / texSize.x, 0.0)).rgb * weights[i];
        }
    }
    else
    {
        for(int i = 1; i < 3; ++i)
        {
            result += texture(TexSrc, TexCoords + vec2(0.0, offset[i] / texSize.x)).rgb * weights[i];
            result += texture(TexSrc, TexCoords - vec2(0.0, offset[i] / texSize.x)).rgb * weights[i];
        }
    }
    FragColor = vec4(result, 1.0);
}