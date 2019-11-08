#version 450 core
in vec4 FragPos;

uniform vec3 lightPos;
uniform float far_plane;

void main()
{
	float lightDistance = length(FragPos.xyz - lightPos);

	float depth = lightDistance / far_plane;

	gl_FragDepth = depth;
};