#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 iTexCoord;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 bTan;
layout (location = 5) in int id;
layout (location = 6) in ivec4 bone_ids;
layout (location = 7) in vec4 weights;

out vec3 fragPos;
out vec2 texCoord;
out mat3 TBN;
out vec4 sunFLP;
out vec4 fragLightPos[10];

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
uniform mat4 corr;

const int MAX_BONES = 50;
const int MAX_VERTS = 6000;

uniform mat4[MAX_BONES] bone_t;
uniform mat4[MAX_BONES] b_axis;
uniform mat4[MAX_BONES] b_axis_i;
uniform mat3[MAX_BONES] norms;
uniform vec3[MAX_BONES] bind_p;
uniform vec3[MAX_BONES] bind_t;
uniform int num_bones;

uniform mat4 sunLSM;
uniform mat4 lightSpaceMatrix[10];
uniform int num_lights;

uniform mat3 normMat;

void main()
{
	vec4 newPos = vec4(0.0, 0.0, 0.0, 0.0);
	vec3 newNorm = vec3(0.0, 0.0, 0.0);

	int id1 = 0;

	if(weights.x + weights.y + weights.z + weights.w > 0){
		newPos += weights.x * (bone_t[bone_ids.x] * (vec4(aPos - bind_p[bone_ids.x], 1.0)));
		newPos += weights.y * (bone_t[bone_ids.y] * (vec4(aPos - bind_p[bone_ids.y], 1.0)));
		newPos += weights.z * (bone_t[bone_ids.z] * (vec4(aPos - bind_p[bone_ids.z], 1.0)));
		newPos += weights.w * (bone_t[bone_ids.w] * (vec4(aPos - bind_p[bone_ids.w], 1.0)));

		//newPos += b_axis[bone_ids.x] * ( bone_t[bone_ids.x] * (b_axis_i[bone_ids.x] * vec4(aPos - bind_p[bone_ids.x], 1.0)));
		
		//newPos += (bone_t[bone_ids.x] * (vec4(aPos - bind_p[bone_ids.x], 1.0)));

		newNorm += weights.x * norms[bone_ids.x] * aNorm;
		newNorm += weights.y * norms[bone_ids.y] * aNorm;
		newNorm += weights.z * norms[bone_ids.z] * aNorm;
		newNorm += weights.w * norms[bone_ids.w] * aNorm;

	}else{
		newPos = vec4(aPos, 1.0);
		newNorm = aNorm;
	}

	fragPos = vec3(model * newPos); 
	texCoord = iTexCoord;

	vec3 t = normalize(normMat * aTan);
	vec3 b = normalize(normMat * bTan);
	vec3 n = normalize(normMat * newNorm);

	TBN = mat3(t,b,n);

	for(int l = 0; l < num_lights; l++){
		fragLightPos[l] =  corr * lightSpaceMatrix[l] * vec4(fragPos, 1); 
	}
	sunFLP = corr * sunLSM * vec4(fragPos, 1);

    gl_Position = corr * project * view * model * newPos;
	//gl_Position = newPos;
}