#version 330 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 1) in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D INPUT_0;
uniform sampler3D lut;

uniform int lut_size;

void main(){
	vec3 c = texture2D(INPUT_0, texCoord).rgb;

	vec3 size = textureSize(lut, 0);
	vec3 lut_color = texture(lut, clamp((vec3(0.5) + (c * (size - vec3(1.0)))) / size, vec3(0.0), vec3(1.0))).rgb;

	fragColor = vec4(lut_color, 1.0);
}