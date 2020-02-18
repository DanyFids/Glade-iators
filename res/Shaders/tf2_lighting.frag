#version 450 core
// TF2 Lighting Shader by Daniel Findleton.

out vec4 FragColor;
in vec3 fragPos;
in mat3 TBN;
in vec2 texCoord;
in vec4 sunFLP;
in vec4 fragLightPos[10];

struct Material{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D normal;
	float shine;
};

struct DirLight{
	vec3 pos;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	sampler2D depthMap;
};

struct PointLight{
	vec3 position;

	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	samplerCube depthMap;
};

uniform vec3 viewPos;
uniform Material material;
uniform DirLight sun;
uniform PointLight lights[2];
uniform int num_lights;
uniform vec4 indexColor;

uniform bool enable_a;
uniform bool enable_d;
uniform bool enable_s;
uniform bool enable_r;

float shadow_bias = 5;

uniform float far_plane;

float SunShadow(vec4 fragInLightSpace){
	vec3 projCoords = fragInLightSpace.xyz / fragInLightSpace.w;

	projCoords = projCoords * 0.5 + 0.5;

	vec2 texelSize = 1.0 / textureSize(sun.depthMap, 0);

	float shadow = 0.0;
	for(int x = -1; x <= 1; ++x){
		for(int y = -1; y <= 1; ++y){
			float pcfDepth = texture(sun.depthMap, fragInLightSpace.xy + vec2(x,y) * texelSize).r;
			shadow += projCoords.z - shadow_bias > pcfDepth ? 1.0 : 0.0;
		}
	}

	shadow = shadow/9.0;

	float lightDepth = texture(sun.depthMap, projCoords.xy).r;
	float curdepth = projCoords.z;
	
	shadow = curdepth - shadow_bias > lightDepth ? 1.0 : 0.0;

	return shadow;
}

float ShadowCalc(vec3 fragPos, int light_id){
	float shadowPen = 0.05;

	vec3 fragToLight = fragPos - lights[light_id].position;
	fragToLight = fragToLight * vec3(1, -1, -1);
	float closestDepth = texture(lights[light_id].depthMap, normalize(fragToLight)).r;
	closestDepth *= far_plane;

	float curDepth = length(fragToLight);

	float shadow;

	if ((curDepth - shadowPen) < closestDepth)
	    shadow = 1.0;
	else
	    shadow = 0.0;

	return shadow;
}

uniform sampler2D lut;

//float fuckMe = 0.5;

void main(){
	vec3 objColor = vec3(texture(material.diffuse, texCoord));
	vec3 specVal = vec3(texture(material.specular, texCoord));
	vec3 normTex = vec3(texture(material.normal, texCoord));

	vec3 vi = normalize(viewPos - fragPos);

	vec4 kd = texture(material.diffuse, texCoord);

	vec3 norm = (normTex * 2) - 1;
	norm = normalize(TBN * norm);

	vec3 iLightTot = vec3(0.0);
	vec3 dLightTot = vec3(0.0);

	float fr = pow(1 - dot(norm, vi), 4);

	for(int i = 0; i < num_lights; i++){
		vec3 lightDir = normalize(lights[i].position - fragPos);
		//vec3 halfWay = normalize(viewDir + lightDir);

		vec3 an = vec3(0.0);

		if(enable_a)
			an = lights[i].ambient * (dot(norm, lightDir));

		vec3 C = lights[i].diffuse;

		float d_enab = 0.0;
		if(enable_d)
			d_enab = 1.0;

		vec3 w = vec3(0.9, 0.875, 0.875);
		float a = 0.5;
		float b = 0.5;

		float ni = dot(norm, lightDir);

		float y = 1;

		//iLightTot +=  ShadowCalc(fragPos, i) * C * w *(pow((a * ni + b ), y)) + an;
		iLightTot += ShadowCalc(fragPos, i) * C * d_enab * w *(pow((a * ni + b ), y)) + an;

		vec3 ks = vec3(texture(material.specular, texCoord));

		vec3 ri = reflect(-lightDir, norm);

		float f_term1 = 0;
		if(enable_s){
			f_term1 = 0.5 * pow(dot(vi, ri), 10);
		}

		float f_term2 = 0;
		if(enable_r){
			f_term2 = fr * 0.5 * pow(dot(vi, ri), 1);
		}

		dLightTot += ShadowCalc(fragPos, i) *C * ks * max(f_term1, f_term2);

		if(enable_a){
			dLightTot += dot(norm, vec3(0.0, 1.0, 0.0)) * fr * 1.0 * (lights[i].ambient) * (dot(vi, normalize(lights[i].position - fragPos) + 0.1));
		}
	}

	vec3 indirectColor = vec3(kd) * (iLightTot);
	vec3 directColor = dLightTot;

	vec3 result = indirectColor + directColor;

	// Gamma it up
	//result = pow(result, vec3(1.0/1.2));

	FragColor = vec4(result, 1.0);

}