#version 450 core
in vec2 outUV;

out vec4 FragColor;

uniform sampler2D tex; 
uniform vec3 indexColor;

void main(){
	vec3 objColor = vec3(texture(tex, outUV));

	if(objColor.r == objColor.b && objColor.g == 0.0){
		objColor.g = objColor.r;
		objColor = objColor * vec3(indexColor);
	}

	FragColor = vec4(objColor, 1.0);
}