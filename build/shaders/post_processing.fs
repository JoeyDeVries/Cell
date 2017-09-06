#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D TexSrc;

void main()
{
    vec3 color = texture(TexSrc, TexCoords).rgb;
    
    // HDR tonemapping
    float exposure = 1.0;
	color = vec3(1.0) - exp(-color * exposure);	
	// gamma correct
	color = pow(color, vec3(1.0/2.2));     

    FragColor = vec4(color, 1.0);
}