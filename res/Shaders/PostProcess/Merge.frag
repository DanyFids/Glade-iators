#version 430 core
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT_0;
uniform sampler2D INPUT_1;
uniform sampler2D INPUT_2;

void main(){
	vec4 col1 = texture(INPUT_0, texCoord);
	vec4 col2 = texture(INPUT_1, texCoord);
	vec4 col3 = texture(INPUT_2, texCoord);

	fragColor = (1.0 - col3.a)*(col1 * col2) + col3;
}