#version 450 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Normals;
layout (location = 2) out vec4 Specular;

in vec3 fragPos;
in mat3 TBN;
in vec2 texCoord;
in vec4 sunFLP;
in vec4 fragLightPos[10];

uniform vec4 indexColor;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shine;
};

uniform Material material;

void main(){
	vec3 objColor = vec3(texture(material.diffuse, texCoord));
	vec3 specVal = vec3(texture(material.specular, texCoord));
	vec3 normTex = vec3(texture(material.normal, texCoord));
	
	if(objColor.r == objColor.b && objColor.g == 0.0){
		objColor.g = objColor.r;
		objColor = objColor * vec3(indexColor);
	}

	vec3 norm = (normTex * 2) - 1;
	vec3 normal = normalize(TBN * norm);

	vec3 outNorm = (normal / 2) + 0.5;

	FragColor = vec4(objColor, 1.0);
	Normals = vec4(outNorm, 1.0);
	Specular = vec4(specVal, material.shine / 256);
}