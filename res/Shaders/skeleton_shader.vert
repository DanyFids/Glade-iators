#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 iTexCoord;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 bTan;
layout (location = 5) in int id;

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

uniform mat4[MAX_BONES] bone_t;
uniform int num_bones;
uniform sampler2D weightMap;

uniform mat4 sunLSM;
uniform mat4 lightSpaceMatrix[10];
uniform int num_lights;

uniform mat3 normMat;

void main()
{
	vec4 newPos = vec4(0.0, 0.0, 0.0, 0.0);

	vec2 wm_size = textureSize(weightMap, 0);

	vec2 texel_s = vec2(1.0/wm_size.x, 1.0/wm_size.y); 

	for(int c = 0; c < num_bones - 1; c++){
		float w = texture(weightMap, vec2(c * texel_s.x, id * texel_s.y)).r;

		if(w > 0.0){
			newPos += bone_t[c] * vec4(aPos, 1.0) * w;
		}
	}

	fragPos = vec3(model * vec4(aPos, 1.0)); 
	texCoord = iTexCoord;

	vec3 t = normalize(normMat * aTan);
	vec3 b = normalize(normMat * bTan);
	vec3 n = normalize(normMat * aNorm);

	TBN = mat3(t,b,n);

	for(int l = 0; l < num_lights; l++){
		fragLightPos[l] =  corr * lightSpaceMatrix[l] * vec4(fragPos, 1); 
	}
	sunFLP = corr * sunLSM * vec4(fragPos, 1);

    gl_Position = corr * project * view * model * newPos;
}