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

// sepia
const vec3 sepiaColor = vec3(1.2, 1.0, 0.8);

// vignette

// bloom
uniform sampler2D TexBloom1;
uniform sampler2D TexBloom2;
uniform sampler2D TexBloom3;
uniform sampler2D TexBloom4;

// ssr
uniform sampler2D TexSSR;

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    vec3 grayscale = vec3(dot(color, vec3(0.299, 0.587, 0.114)));
       
    if(Bloom)
    {
        const float strength = 1.0;
        vec3 bloom1 = texture(TexBloom1, TexCoords).rgb * strength;
        vec3 bloom2 = texture(TexBloom1, TexCoords).rgb * strength;
        vec3 bloom3 = texture(TexBloom1, TexCoords).rgb * strength;
        vec3 bloom4 = texture(TexBloom1, TexCoords).rgb * strength;
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
	// color = vec3(1.0) - exp(-color * exposure);	
    color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     
    
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