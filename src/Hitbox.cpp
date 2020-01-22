#include "Hitbox.h"
#include "Object.h"

CubeHitbox::CubeHitbox(float w, float h, float d):dim(glm::vec3(w,h,d))
{
	

}

bool CubeHitbox::HitDetect(Transform t, CubeHitbox* other, Transform oT) // AABB
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
	float ohw = dim.x / 2 * t.scale.x;
	float ohh = dim.y / 2 * t.scale.y;
	float ohd = dim.z / 2 * t.scale.z;

	glm::vec3 closestPoint = glm::vec3(
		glm::max(t.position.x - ohw, glm::min(oT.position.x, t.position.x + ohw)),
		glm::max(t.position.y - ohh, glm::min(oT.position.y, t.position.y + ohh)),
		glm::max(t.position.z - ohd, glm::min(oT.position.z, t.position.z + ohd))
	);

	float distance = glm::distance(oT.position, closestPoint);

	if (distance < other->GetRadius())
		return true;

	return false;
}

bool CubeHitbox::HitDetect(Transform t, CapsuleHitbox* other, Transform oT)
{
	return false;
}

bool CubeHitbox::HitDetectSAT(Transform t, CubeHitbox* other, Transform oT)
{
	this->setVerts(t);
	this->setNorms();

	other->setVerts(oT);
	other->setNorms();

	return testIntersection(t,other,oT);
}

//////////////// 
//			For SAT Hit detection
/////////////

void CubeHitbox::setVerts(Transform t)
{
	vertices.clear();

	float halfW = GetDim().x / 2 * t.scale.x;
	float halfH = GetDim().y / 2 * t.scale.y;
	float halfD = GetDim().z / 2 * t.scale.z;

	vertices.push_back(glm::vec3(t.position.x + halfW, t.position.y + halfH, t.position.z + halfD) * t.GetRot); //right, top, front , 0 * (norm 1)
	vertices.push_back(glm::vec3(t.position.x - halfW, t.position.y + halfH, t.position.z + halfD) * t.GetRot); //left, top, front , 1 * (norm 2)

	vertices.push_back(glm::vec3(t.position.x + halfW, t.position.y + halfH, t.position.z - halfD) * t.GetRot); //right, top, back , 2
	vertices.push_back(glm::vec3(t.position.x - halfW, t.position.y + halfH, t.position.z - halfD) * t.GetRot); //left, top, back , 3 * (norm 1) (norm3)

	vertices.push_back(glm::vec3(t.position.x + halfW, t.position.y - halfH, t.position.z + halfD) * t.GetRot); //right, bottom, front , 4
	vertices.push_back(glm::vec3(t.position.x - halfW, t.position.y - halfH, t.position.z + halfD) * t.GetRot); //left, bottom, front , 5

	vertices.push_back(glm::vec3(t.position.x + halfW, t.position.y - halfH, t.position.z - halfD) * t.GetRot); //right, bottom, back , 6 (norm 3)
	vertices.push_back(glm::vec3(t.position.x - halfW, t.position.y - halfH, t.position.z - halfD) * t.GetRot); //left, bottom, back , 7(norm 2)


}

void CubeHitbox::setNorms()
{
	normals.clear();

	normals.push_back(glm::cross(vertices[0], vertices[3]));
	normals.push_back(glm::cross(vertices[2], vertices[7]));
	normals.push_back(glm::cross(vertices[6], vertices[3]));
}

void CubeHitbox::GetInterval(Transform t, glm::vec3 axis, float& min, float& max)
{
	setVerts(t);

	min = max = glm::dot(axis, vertices[0]);
	for (int i = 0; i < vertices.size(); i++) {
		float value = glm::dot(axis, vertices[i]);
		min = glm::min(min, value);
		max = glm::max(max, value);
	}

}

bool CubeHitbox::testIntersection(Transform t, CubeHitbox* object2, Transform oT)
{
	float min1, max1, min2, max2;

	for (int i = 0; this->normals.size(); i++) {
		GetInterval(t, this->normals[i], min1, max1);
		GetInterval(oT, this->normals[i], min2, max2);
		if (max1 < max2 || max2 < min1) {
			return false;
		}
	}

	for (int i = 0; object2->normals.size(); i++) {
		GetInterval(t, object2->normals[i], min1, max1);
		GetInterval(oT, object2->normals[i], min2, max2);
		if (max1 < max2 || max2 < min1) {
			return false;
		}
	}

	//Get EdgeDirs?

	return true;
}

/********************************************************************
*						SPHERE COLLISION
********************************************************************/

bool SphereHitbox::HitDetect(Transform t, CubeHitbox* other, Transform oT)
{
	float ohw = other->GetDim().x / 2 * oT.scale.x;
	float ohh = other->GetDim().y / 2 * oT.scale.y;
	float ohd = other->GetDim().z / 2 * oT.scale.z;

	glm::vec3 closestPoint = glm::vec3(
		glm::max(oT.position.x - ohw, glm::min(t.position.x, oT.position.x + ohw)),
		glm::max(oT.position.y - ohh, glm::min(t.position.y, oT.position.y + ohh)),
		glm::max(oT.position.z - ohd, glm::min(t.position.z, oT.position.z + ohd))
	);

	float distance = glm::distance(t.position, closestPoint);
	
	if (distance < radius)
		return true;

	return false;
}

bool SphereHitbox::HitDetect(Transform t, SphereHitbox* other, Transform oT)
{
	float distance = glm::distance(t.position, oT.position);
	float maxDistance = radius * (t.scale.x + t.scale.y + t.scale.z) / 3 + other->GetRadius() * (oT.scale.x + oT.scale.y + oT.scale.z) / 3;
	
	if (distance < maxDistance)
		return true;
	return false;
}

bool SphereHitbox::HitDetect(Transform t, CapsuleHitbox* other, Transform oT)
{
	return false;
}
