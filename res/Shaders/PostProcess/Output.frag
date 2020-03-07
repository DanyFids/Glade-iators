#version 450 core
layout (location = 1) in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT;

void main(){
	fragColor = texture2D(INPUT, texCoord);
}