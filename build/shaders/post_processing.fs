#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

// post-process effect toggles
uniform int Vignette;
uniform int Sepia;
uniform int Bloom;
uniform int SSAO;
uniform int SSR;
uniform int MotionBlur;

// sepia
const vec3 sepiaColor = vec3(1.2, 1.0, 0.8);

// vignette

// bloom
uniform sampler2D TexBloom1;
uniform sampler2D TexBloom2;
uniform sampler2D TexBloom3;
uniform sampler2D TexBloom4;

// motion blur
uniform sampler2D gMotion;
uniform float MotionScale;
uniform int MotionSamples;

// ssr
uniform sampler2D TexSSR;

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    vec2 texelSize = 1.0 / textureSize(TexSrc, 0).xy;    
        
    if(MotionBlur)
    {
        vec2 motion = texture(gMotion, TexCoords).xy;
        motion     *= MotionScale;
        
        vec3 avgColor = color;
        for(int i = 0; i < MotionSamples; ++i)
        {
            vec2 offset = motion * (float(i) / float(MotionSamples - 1) - 0.5);
            avgColor += texture(TexSrc, TexCoords + offset).rgb;
        }
        avgColor /= float(MotionSamples);
        color = avgColor;
    }    
       
    if(Bloom)
    {
        const float strength = 0.5;
        vec3 bloom1 = texture(TexBloom1, TexCoords).rgb * strength;
        vec3 bloom2 = texture(TexBloom2, TexCoords).rgb * strength * 0.5;
        vec3 bloom3 = texture(TexBloom3, TexCoords).rgb * strength * 0.25;
        vec3 bloom4 = texture(TexBloom4, TexCoords).rgb * strength * 0.125;
        color += bloom1;
        color += bloom2;
        color += bloom3;
        color += bloom4;
    }

    if(SSR)
    {
        vec3 ssrColor = texture(TexSSR, TexCoords).rgb;
        color += ssrColor;
    }
    
    // HDR tonemapping
    float exposure = 1.0;
    color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     
    // color *= 2.0;  // Hardcoded Exposure Adjustment
    // vec3 x = max(vec3(0.0), color - 0.004);
    // color = (x * (6.2 * x + 0.5)) / (x * (6.2 * x + 1.7) + 0.06);
    
    if(Sepia)
    {
        color = mix(color, grayscale * sepiaColor, 0.7);
    }
    if(Vignette)
    {
        const float strength = 10.0;
        const float power = 0.1;
        vec2 tuv = TexCoords * (vec2(1.0) - TexCoords.yx);
        float vign = tuv.x*tuv.y * strength;
        vign = pow(vign, power);
        color *= vign;
    }

    FragColor = vec4(color, 1.0);
}