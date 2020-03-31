#version 410 core
layout (location = 1) in vec2 TexCoords;

out vec4 color;

uniform sampler2D tex;

void main()
{
    color = (texture(tex, TexCoords));
}  