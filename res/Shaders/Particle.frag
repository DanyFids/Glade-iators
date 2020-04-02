#version 430 core
layout (location = 1) in vec2 TexCoords;
layout (location = 2) flat in int _var;
layout (location = 3) flat in int _fra;

const int MAX_VAR = 4;
const int MAX_FRAMES = 4;

out vec4 color;

uniform sampler2D[MAX_VAR][MAX_FRAMES] tex;

void main()
{
	int v = int(_var);
	int f = int(_fra);

    vec4 col = (texture(tex[v][f], TexCoords));

	if(col.a == 0.0)
		discard;
	else
		color = col;
}  