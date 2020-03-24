#version 330 core
#extension GL_ARB_separate_shader_objects : enable
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT_0;
uniform sampler2D INPUT_1;
uniform sampler2D INPUT_2;

void main(){
	vec4 col1 = texture(INPUT_0, texCoord);
	vec4 col2 = texture(INPUT_1, texCoord);
	vec4 col3 = texture(INPUT_2, texCoord);

	fragColor = col1 * col2 + col3;
}