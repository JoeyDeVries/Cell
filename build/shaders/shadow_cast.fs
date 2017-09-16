#version 330 core
out vec4 FragColor;

void main()
{
    // FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}