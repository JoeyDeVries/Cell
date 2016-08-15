#version 330 core
out vec4 FragColor;
in vec3 color;
in vec2 TexCoords;

#include test_include.glsl

uniform float testUniform;
uniform sampler2D testTexture;

void main()
{
	FragColor = texture(testTexture, TexCoords) * vec4(color, 1.0f);
	// FragColor = vec4(TexCoords, 0.0f, 1.0f);
}
