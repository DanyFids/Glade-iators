#include "Hitbox.h"
#include "Object.h"

CubeHitbox::CubeHitbox(float w, float h, float d):dim(glm::vec3(w,h,d))
{
}

bool CubeHitbox::HitDetect(Transform t, CubeHitbox* other, Transform oT)
{
	float halfW = GetDim().x / 2 * t.scale.x;
	float halfH = GetDim().y / 2 * t.scale.y;
	float halfD = GetDim().z / 2 * t.scale.z;

	float ohw = other->GetDim().x / 2 * oT.scale.x;
	float ohh = other->GetDim().y / 2 * oT.scale.y;
	float ohd = other->GetDim().z / 2 * oT.scale.z;

	if (t.position.x - halfW < oT.position.x + ohw && t.position.x + halfW > oT.position.x - ohw &&
		t.position.y - halfH < oT.position.y + ohh && t.position.y + halfH > oT.position.y - ohh &&
		t.position.z - halfD < oT.position.z + ohd && t.position.z + halfD > oT.position.z - ohd) {
		return true;
	}else{ 
		return false; 
	}
}

bool CubeHitbox::HitDetect(Transform t, SphereHitbox* other, Transform oT)
{
	return false;
}

bool CubeHitbox::HitDetect(Transform t, CapsuleHitbox* other, Transform oT)
{
	return false;
}


