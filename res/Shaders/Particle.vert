#version 330 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 texUv;

out vec2 texCoords;

void main()
{
	texCoords = texUv;
    gl_Position = vec4(Pos, 1.0);
}