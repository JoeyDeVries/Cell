#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 LocalPos;
out vec3 WorldPos;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 Position;

void main()
{
    LocalPos = aPos * 0.5;
    WorldPos = LocalPos + Position;
	gl_Position = projection * view * vec4(WorldPos, 1.0);
}