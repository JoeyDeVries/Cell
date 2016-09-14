#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;

out vec3 color;
out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


float time;

void main()
{
	color = pos;
	TexCoords = texCoords;
	
	gl_Position =  projection * view * model * vec4(pos, 1.0f);
}