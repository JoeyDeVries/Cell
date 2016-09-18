#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;

out vec3 color;
out vec2 TexCoords;
out vec3 Normal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


float time;

void main()
{
	color = pos;
	TexCoords = texCoords;
	Normal = normal;
	
	gl_Position =  projection * view * model * vec4(pos, 1.0f);
}