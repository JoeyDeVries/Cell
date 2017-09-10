#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

// post-process effect toggles
uniform int vignette;
uniform int Bloom;
uniform int SSAO;

// settings: bloom

// settings: ssao

// settings: vignette

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    
    if(vignette)
    {
        // color *= vec3(1.0, 0.0, 0.0);
    }
    
    
    // HDR tonemapping
    float exposure = 1.0;
	// color = vec3(1.0) - exp(-color * exposure);	
    color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     

    FragColor = vec4(color, 1.0);
}