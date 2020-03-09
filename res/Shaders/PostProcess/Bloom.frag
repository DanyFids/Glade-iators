#version 450 core
layout (location = 1) in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT_0;
uniform sampler2D INPUT_1;

void main(){
	vec4 c1 = texture(INPUT_0, texCoord);
	vec4 c2 = texture(INPUT_1, texCoord);

	fragColor = c1 + c2;
}