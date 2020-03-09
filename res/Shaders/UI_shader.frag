#version 330 core
in vec2 outUV;

out vec4 FragColor;

uniform float opacity;

uniform sampler2D tex; 
uniform vec3 indexColor;

void main(){
	vec4 objColor = texture(tex, outUV);

	if(objColor.r == objColor.b && objColor.g == 0.0){
		objColor.g = objColor.r;
		objColor = objColor * vec4(indexColor, 1.0);
	}

	FragColor = vec4(objColor.xyz, objColor.a * opacity);
}