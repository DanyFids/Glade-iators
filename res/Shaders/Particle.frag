#version 330 core
in vec2 TexCoords;
in vec4 Color;

out vec4 color;

uniform Material tex;

void main()
{
    color = (texture(tex, TexCoords) * Color);
}  