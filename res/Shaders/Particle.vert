#version 430 core
layout (location = 0) in vec3 Pos;
layout (location = 1) in vec2 Size;
layout (location = 2) in int variation;
layout (location = 3) in int frame;

layout (location = 0) out vec2 outSize;
layout (location = 1) flat out int outVar;
layout (location = 2) flat out int outFrame;

void main()
{
    gl_Position = vec4(Pos, 1.0);
    outSize = Size;
	outVar = variation;
	outFrame = frame;
}