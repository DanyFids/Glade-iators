#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 1) in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT;

void main(){
	fragColor = texture2D(INPUT, texCoord);
}