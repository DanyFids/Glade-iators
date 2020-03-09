#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 0) in vec3 pos;
layout (location = 2) in vec2 texUV;

out vec2 texCoord;


void main(){
	texCoord = texUV;
	gl_Position = vec4(pos, 1);

}