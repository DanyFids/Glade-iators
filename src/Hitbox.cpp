#include "Hitbox.h"
#include "Object.h"
#include <iostream>

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
	this->setEdgeDirs();

	other->setVerts(oT);
	other->setNorms();
	other->setEdgeDirs();

	glm::vec3 offset = oT.position - t.position;

	return testIntersection(t, other, oT, offset);
}

//////////////// 
//			For SAT Hit detection
/////////////
#pragma region SAT HD STUFF
void CubeHitbox::setVerts(Transform t)
{
	this->vertices.clear();

	float halfW = GetDim().x / 2 * t.scale.x;
	float halfH = GetDim().y / 2 * t.scale.y;
	float halfD = GetDim().z / 2 * t.scale.z;


	this->vertices.push_back(glm::vec4(t.position.x + halfW, t.position.y + halfH, t.position.z + halfD,1) * t.GetRot()); //right, top, front , 0 * (norm 1)
	this->vertices.push_back(glm::vec4(t.position.x - halfW, t.position.y + halfH, t.position.z + halfD,1) * t.GetRot()); //left, top, front , 1 * (norm 2)

	this->vertices.push_back(glm::vec4(t.position.x + halfW, t.position.y + halfH, t.position.z - halfD,1) * t.GetRot()); //right, top, back , 2
	this->vertices.push_back(glm::vec4(t.position.x - halfW, t.position.y + halfH, t.position.z - halfD,1) * t.GetRot()); //left, top, back , 3 * (norm 1) (norm3)

	this->vertices.push_back(glm::vec4(t.position.x + halfW, t.position.y - halfH, t.position.z + halfD,1) * t.GetRot()); //right, bottom, front , 4
	this->vertices.push_back(glm::vec4(t.position.x - halfW, t.position.y - halfH, t.position.z + halfD,1) * t.GetRot()); //left, bottom, front , 5
							   
	this->vertices.push_back(glm::vec4(t.position.x + halfW, t.position.y - halfH, t.position.z - halfD,1) * t.GetRot()); //right, bottom, back , 6 (norm 3)
	this->vertices.push_back(glm::vec4(t.position.x - halfW, t.position.y - halfH, t.position.z - halfD,1) * t.GetRot()); //left, bottom, back , 7(norm 2)

}

void CubeHitbox::setNorms()
{
	this->normals.clear();
	
	this->normals.push_back(glm::cross(convertVec4((this->vertices[0] - this->vertices[2])),convertVec4((this->vertices[3] - this->vertices[2]) )) * glm::vec3(-1) ); //TopDown normal
	this->normals.push_back(glm::cross(convertVec4((this->vertices[0] - this->vertices[2])), convertVec4((this->vertices[6] - this->vertices[2])) )); //RightLeft normal
	this->normals.push_back(glm::cross(convertVec4((this->vertices[3] - this->vertices[2])), convertVec4((this->vertices[6] - this->vertices[2])) )); //FrontBack Normal

	this->normals.push_back(glm::cross(convertVec4((this->vertices[0] - this->vertices[2])), convertVec4((this->vertices[3] - this->vertices[2])))); //TopDown normal
	this->normals.push_back(glm::cross(convertVec4((this->vertices[0] - this->vertices[2])), convertVec4((this->vertices[6] - this->vertices[2]))) * glm::vec3(-1)); //RightLeft normal
	this->normals.push_back(glm::cross(convertVec4((this->vertices[3] - this->vertices[2])), convertVec4((this->vertices[6] - this->vertices[2]))) * glm::vec3(-1)); //FrontBack Normal
}

void CubeHitbox::setEdgeDirs()
{	
	this->edgeDirs.clear();

	//Supplementary angle vectors based around rotation for the purpose of finding normals
	glm::vec3 upVec = glm::normalize(this->vertices[0] - this->vertices[4]);
	glm::vec3 downVec = upVec * glm::vec3(-1,-1,-1);

	glm::vec3 rightVec = glm::normalize(this->vertices[0] - this->vertices[1]);
	glm::vec3 leftVec = rightVec * glm::vec3(-1);

	glm::vec3 backVec = glm::normalize(this->vertices[3] - this->vertices[1]);
	glm::vec3 frontVec = backVec * glm::vec3(-1);

	//Top edges
#pragma region Tops

	{//Top Front Edge
		glm::vec3 p1 = glm::normalize(vertices[3] - vertices[1]) - glm::normalize(vertices[5] - vertices[1]); //Diagonal, from the corner.
		glm::vec3 p2 = rightVec;


		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

	{//Top Left Edge
		glm::vec3 p1 = glm::normalize(vertices[0] - vertices[1]) - glm::normalize(vertices[5] - vertices[1]); //Diagonal, from the corner.
		glm::vec3 p2 = backVec;


		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

	{//Top Right Edge
		glm::vec3 p1 = glm::normalize(vertices[3] - vertices[2]) - glm::normalize(vertices[6] - vertices[2]); //Diagonal, from the corner.
		glm::vec3 p2 = frontVec;


		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

	{//Top Back Edge
		glm::vec3 p1 = glm::normalize(vertices[0] - vertices[2]) - glm::normalize(vertices[6] - vertices[2]); //Diagonal, from the corner.
		glm::vec3 p2 = leftVec;


		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

#pragma endregion
	//Vertical edges
#pragma region verticals

	//Front Right Top Edge Vertical
	{
		glm::vec3 p1 = glm::normalize(vertices[1] - vertices[0]) - glm::normalize(vertices[2] - vertices[0]);
		glm::vec3 p2 = downVec;
		
		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

	//Front Left Top Edge Vertical
	{					
		glm::vec3 p1 = glm::normalize(vertices[0] - vertices[1]) - glm::normalize(vertices[3] - vertices[1]); //Diagonal, from the corner.
		glm::vec3 p2 = downVec;
	

		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

	//Back Left Top Edge Vertical
	{
		glm::vec3 p1 = glm::normalize(vertices[1] - vertices[3]) - glm::normalize(vertices[2] - vertices[3]); //Diagonal, from the corner.
		glm::vec3 p2 = downVec;


		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

	//Back Right Top Edge Vertical
	{
		glm::vec3 p1 = glm::normalize(vertices[0] - vertices[2]) - glm::normalize(vertices[3] - vertices[2]); //Diagonal, from the corner.
		glm::vec3 p2 = downVec;


		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

#pragma endregion

	//Bottom edges.
#pragma region Bots


	{//Bot Front Edge
		glm::vec3 p1 = glm::normalize(vertices[1] - vertices[5]) - glm::normalize(vertices[7] - vertices[5]); //Diagonal, from the corner.
		glm::vec3 p2 = rightVec;


		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

	{//Bot Left Edge
		glm::vec3 p1 = glm::normalize(vertices[1] - vertices[5]) - glm::normalize(vertices[4] - vertices[5]); //Diagonal, from the corner.
		glm::vec3 p2 = backVec;


		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

	{//Bot Right Edge
		glm::vec3 p1 = glm::normalize(vertices[7] - vertices[6]) - glm::normalize(vertices[2] - vertices[6]); //Diagonal, from the corner.
		glm::vec3 p2 = frontVec;


		this->edgeDirs.push_back(glm::cross(p2, p1));
	}

	{//Bot Back Edge
		glm::vec3 p1 = glm::normalize(vertices[4] - vertices[6]) - glm::normalize(vertices[2] - vertices[6]); //Diagonal, from the corner.
		glm::vec3 p2 = leftVec;


		this->edgeDirs.push_back(glm::cross(p1, p2));
	}

#pragma endregion
}

glm::vec3 CubeHitbox::convertVec4(glm::vec4 _vec4)
{
	glm::vec3 pass{ _vec4.x,_vec4.y,_vec4.z };

	return pass;
}

void CubeHitbox::GetInterval(CubeHitbox* object, glm::vec3 axis, float& min, float& max)
{
	//setVerts(t);

	min = max = glm::dot(axis, convertVec4(object->vertices[0]));
	for (int i = 0; i < object->vertices.size(); i++) {
		float value = glm::dot(axis, convertVec4(object->vertices[i]));
		min = glm::min(min, value);
		max = glm::max(max, value);
	}

}

bool CubeHitbox::testIntersection(Transform t, CubeHitbox* object2, Transform oT, glm::vec3 offset)
{
	float min1, max1, min2, max2;

	//First two for loops are for checking basic normal based hyperplane check
	for (int i = 0; i < this->normals.size(); i++) {
		GetInterval(this, this->normals[i], min1, max1);

		GetInterval(object2, this->normals[i], min2, max2);
		if (max1 < max2 || max2 < min1) {
			return false;
		}
	}

	for (int i = 0; i < object2->normals.size(); i++) {
		GetInterval(this, object2->normals[i], min1, max1);
		GetInterval(object2, object2->normals[i], min2, max2);
		if (max1 < max2 || max2 < min1) {
			return false;
		}
	}

	//Check between pairs of edges of both objects 
	for (int i = 0; i < this->edgeDirs.size(); i++)
	{
		for (int j = 0; j < object2->edgeDirs.size(); j++) {

			glm::vec3 axis = glm::cross(this->edgeDirs[i], object2->edgeDirs[j]); // The axis we are testing
			GetInterval(this, axis, min1, max1);
			GetInterval(object2, axis, min2, max2);
			if (max1 < min2 || max2 < min1) {
				return false;
			}

		}
	}

	return true;
}
#pragma endregion

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

#pragma region Capsule Collisions
glm::vec3 CapsuleHitbox::convertVec4(glm::vec4 _vec4)
{
	glm::vec3 pass{ _vec4.x,_vec4.y,_vec4.z };

	return pass;
}

bool CapsuleHitbox::HitDetect(Transform t, CapsuleHitbox* other, Transform oT)
 {

	//Adding an indiscriminant value to 1 so we don't get destroyed by dividing by zero.

	if (this->height == 1.0f) {
		height += 0.000001f;
	}

	if (other->height == 1.0f) {
		other->height += 0.000001f;
	}


	this->lowerBound = glm::vec4(t.position, 1) * t.GetRot();
	//this->lowerBound = glm::vec4(t.position.x, t.position.y - (height/2), t.position.z, 1) * t.GetRot();
	this->upperBound = glm::vec4(t.position.x, t.position.y + height, t.position.z, 1) * t.GetRot();

	other->lowerBound = glm::vec4(oT.position, 1) * oT.GetRot();
	//other->lowerBound = glm::vec4(oT.position.x, oT.position.y - (height/2), oT.position.z, 1) * oT.GetRot();
	other->upperBound = glm::vec4(oT.position.x, oT.position.y + height, oT.position.z, 1) * oT.GetRot();

	glm::vec3 closestPointA;
	glm::vec3 closestPointB;

	//Find closest point on both lines
	{
		glm::vec3 LineA = convertVec4( this->upperBound) - convertVec4( this->lowerBound);
		glm::vec3 LineB = convertVec4( other->upperBound) - convertVec4( other->lowerBound);

		glm::vec3 PA1 = convertVec4(this->lowerBound);
		glm::vec3 PA2 = convertVec4(this->upperBound);

		glm::vec3 PB1 = convertVec4(other->lowerBound);
		glm::vec3 PB2 = convertVec4(other->upperBound);

		glm::vec3 DirA = glm::normalize(LineA);
		glm::vec3 DirB = glm::normalize(LineB);

		//Getting a scalar value for length along line B
		float scalarB = (((glm::dot(LineA,(PB1 - PA1)) * glm::dot(LineA, LineB) / glm::dot(LineA, LineA)) - (glm::dot(LineB, (PB1 - PA1)))	) / (glm::dot(LineB, LineB) - 1));

		//Getting a scalar value for length along line A subbing in t.
		float scalarA = (glm::dot(LineB, (PB1 - PA1)) + glm::dot(LineB, LineB) * scalarB) / glm::dot(LineA, LineB);

		//std::cout << scalarB << std::endl;
		//std::cout << scalarA << std::endl;

		closestPointA = PA1 + ((PA2-PA1) * scalarA);
		closestPointB = PB1 + ((PB2-PB1) * scalarB);

		//closestPointA = PA1 + ((DirA) * s);
		//closestPointB = PB1 + ((DirB) * t);


	}

	float totalDist = glm::distance(closestPointA, closestPointB);
	float totalRadius = this->radius + other->GetRadius();

	if (totalDist < totalRadius) {
		return true;
	}

	return false;
}

bool CapsuleHitbox::HitDetect(Transform t, CubeHitbox* other, Transform oT)
{
	return false;
}

bool CapsuleHitbox::HitDetect(Transform t, SphereHitbox* other, Transform oT)
{
	return false;
}

#pragma endregion