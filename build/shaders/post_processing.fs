#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

// post-process effect toggles
uniform int Vignette;
uniform int Sepia;
uniform int Bloom;
uniform int SSAO;
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

// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 aces(vec3 col)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((col*(a*col+b))/(col*(c*col+d)+e), 0.0, 1.0);
}

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
    vec2 texelSize = 1.0 / textureSize(TexSrc, 0).xy;    
        
    if(MotionBlur == 1)
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
       
    if(Bloom == 1)
    {
        const float strength = 0.5;
        vec3 bloom1 = texture(TexBloom1, TexCoords).rgb * strength * 1.00;
        vec3 bloom2 = texture(TexBloom2, TexCoords).rgb * strength * 0.75;
        vec3 bloom3 = texture(TexBloom3, TexCoords).rgb * strength * 0.50;
        vec3 bloom4 = texture(TexBloom4, TexCoords).rgb * strength * 0.25;
        color += bloom1;
        color += bloom2;
        color += bloom3;
        color += bloom4;
    }
    
    // HDR tonemapping
    const float exposure = 1.0;
    color *= exposure;
    // color = aces(color);
    color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     
    
    if(Sepia == 1)
    {
        color = mix(color, grayscale * sepiaColor, 0.7);
    }
    if(Vignette == 1)
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