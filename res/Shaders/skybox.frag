#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Norm;
layout (location = 2) out vec4 Spec;

in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
	Norm = vec4(0.0,0.0,0.0,1.0);
	Spec = vec4(0.0f,0.0f,0.0f,0.0f);
}