#version 430 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 Normals;
layout (location = 2) out vec4 Specular;

in vec3 fragPos;
in mat3 TBN;
in vec2 texCoord;
in vec4 sunFLP;
in vec4 fragLightPos[10];

uniform vec4 indexColor;

uniform bool CG_DISABLE_TEX;

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shine;
};

uniform Material material;

void main(){
	vec4 objColor;
	if(CG_DISABLE_TEX)
		objColor = vec4(1.0, 1.0, 1.0, 1.0);
	else
		objColor = texture(material.diffuse, texCoord);
	vec3 specVal = vec3(texture(material.specular, texCoord));
	vec3 normTex = vec3(texture(material.normal, texCoord));
	
	if(objColor.r == objColor.b && objColor.g == 0.0){
		objColor.g = objColor.r;
		objColor = objColor * indexColor;
	}

	vec3 norm = (normTex * 2) - 1;
	vec3 normal = normalize(TBN * norm);

	vec3 outNorm = (normal / 2) + 0.5;

	if(objColor.a == 0.0)
		discard;

	FragColor = objColor;
	Normals = vec4(outNorm, 1.0);
	Specular = vec4(specVal, material.shine / 256);
}