#version 450 core

layout (location = 1) in vec2 texCoord;

layout (location = 0) out vec4 fragColor;

uniform sampler2D INPUT_0;

void main()
{
        float Pixels = 4000.0;
        float dx = 10.0 * (1.0 / Pixels);
        float dy = 10.0 * (1.0 / Pixels);
        vec2 Coord = vec2(dx * floor(texCoord.x / dx), dy * floor(texCoord.y / dy));
        fragColor = texture(INPUT_0, Coord);
}