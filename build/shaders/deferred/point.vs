#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec4 ScreenPos;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    ScreenPos = projection * view * vec4(FragPos, 1.0);
	gl_Position = ScreenPos;
}