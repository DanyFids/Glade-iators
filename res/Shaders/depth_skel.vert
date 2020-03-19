#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 6) in ivec4 bone_ids;
layout (location = 7) in vec4 weights;

uniform mat4 model;

const int MAX_BONES = 50;
uniform mat4[MAX_BONES] bone_t;
uniform vec3[MAX_BONES] bind_p;

uniform int num_bones;

void main()
{
	vec4 newPos = vec4(0.0);

	if(weights.x + weights.y + weights.z + weights.w > 0){
		newPos += weights.x * (bone_t[bone_ids.x] * (vec4(aPos - bind_p[bone_ids.x], 1.0)));
		newPos += weights.y * (bone_t[bone_ids.y] * (vec4(aPos - bind_p[bone_ids.y], 1.0)));
		newPos += weights.z * (bone_t[bone_ids.z] * (vec4(aPos - bind_p[bone_ids.z], 1.0)));
		newPos += weights.w * (bone_t[bone_ids.w] * (vec4(aPos - bind_p[bone_ids.w], 1.0)));
	}else{
		newPos = vec4(aPos, 1.0);
	}

    gl_Position = model * newPos;
}