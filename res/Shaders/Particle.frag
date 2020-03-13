#version 330 core
in vec2 TexCoords;

out vec4 color;

uniform sampler2D tex;
uniform ivec2 frame_position;
uniform ivec2 frame_size;

void main()
{
    color = (texture(tex, TexCoords));
}  