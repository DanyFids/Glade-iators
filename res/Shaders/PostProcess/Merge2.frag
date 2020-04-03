#version 430 core
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT_0;
uniform sampler2D INPUT_1;
uniform sampler2D INPUT_2;

uniform sampler2D INPUT_3;
uniform sampler2D INPUT_4;
uniform sampler2D INPUT_5;

void main(){
	vec4 col1;
	vec4 col2;
	vec4 col3;

	if(texCoord.x < 0.5){
		col1 = texture(INPUT_0, texCoord + vec2(0.25, 0.0));
		col2 = texture(INPUT_1, texCoord + vec2(0.25, 0.0));
		col3 = texture(INPUT_2, texCoord + vec2(0.25, 0.0));
	}else{
		col1 = texture(INPUT_3, texCoord - vec2(0.25, 0.0));
		col2 = texture(INPUT_4, texCoord - vec2(0.25, 0.0));
		col3 = texture(INPUT_5, texCoord - vec2(0.25, 0.0));
	}

	fragColor = (1.0 - col3.a)*(col1 * col2) + col3;
}