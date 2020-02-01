#version 450 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;
layout (location = 2) in vec2 iTexCoord;
layout (location = 3) in vec3 aTan;
layout (location = 4) in vec3 bTan;

layout (location = 5) in vec3 aPos2;	
layout (location = 6) in vec3 aNorm2;
layout (location = 7) in vec3 aTan2;
layout (location = 8) in vec3 bTan2;

out VERT_OUT{
	vec3 fragPos;
	vec2 texCoord;
	mat3 TBN;
} vert_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 project;
uniform mat4 corr;

uniform float time;

vec3 lerp(vec3 v1, vec3 v2, float t){
	vec3 n;
	n.x = (1.0 - t)*v1.x + t*v2.x;
	n.y = (1.0 - t)*v1.y + t*v2.y;
	n.z = (1.0 - t)*v1.z + t*v2.z;

	return n;
}

void main()
{
	vec3 lerpPos;
	vec3 lerpNorm;
	vec3 lerpATAN;
	vec3 lerpBTAN;

	lerpPos = lerp(aPos,aPos2,time);
	lerpNorm = lerp(aNorm,aNorm2,time);
	lerpATAN = lerp(aTan,aTan2,time);
	lerpBTAN = lerp(bTan,bTan2,time);

	//lerped pos
    gl_Position = corr * project * view * model * vec4(lerpPos.x, lerpPos.y, lerpPos.z, 1.0);
	vert_out.fragPos = vec3(model * vec4(lerpPos, 1.0)); 

	vert_out.texCoord = iTexCoord;

	//lerped Normals
	vec3 t = normalize(vec3(model * vec4(lerpATAN, 0.0)));
	vec3 b = normalize(vec3(model * vec4(lerpBTAN, 0.0)));
	vec3 n = normalize(vec3(model * vec4(lerpNorm, 0.0)));

	vert_out.TBN = mat3(t,b,n);
}