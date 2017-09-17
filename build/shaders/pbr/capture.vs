#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV0;
layout (location = 2) in vec3 aNormal;

out vec2 UV0;
out vec3 FragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	UV0 = aUV0;
	FragPos = vec3(model * vec4(aPos, 1.0));
        
    vec3 N = normalize(mat3(model) * aNormal);
	gl_Position =  projection * view * vec4(FragPos, 1.0);
}