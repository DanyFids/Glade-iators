#version 330 core

const int MAX_PARTICLES = 512;

layout (location = 0) in vec3 Pos;

uniform vec3[MAX_PARTICLES] pos;
uniform vec2[MAX_PARTICLES] size;

out vec2 outSize;

void main()
{
    gl_Position = vec4(pos[gl_VertexID], 1.0);
    outSize = size[gl_VertexID];
}