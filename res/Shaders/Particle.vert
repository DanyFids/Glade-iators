#version 450 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 texUv;

out vec2 TexCoords;

out vec4 Color;

uniform mat4 project;
uniform vec2 offset;
uniform vec4 color;
uniform float scale;

void main()
{
    float size = scale;
    TexCoords = vertex.zw;
    ParticleColor = color;
    gl_Position = projection * vec4((vertex.xy * scale) + offset, 0.0, 1.0);
}