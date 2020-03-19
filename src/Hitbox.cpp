#include "Hitbox.h"

#include <iostream>

#include "Shader.h"
#include <GLM\gtc\type_ptr.hpp>

CubeHitbox::CubeHitbox(float w, float h, float d):dim(glm::vec3(w,h,d))
{
	

}

bool CubeHitbox::HitDetect(Object* th, CubeHitbox* other, Object* oth) // AABB
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();
	oT.position += oth->phys.move;
	t.position += th->phys.move;

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

bool CubeHitbox::HitDetect(Object* th, SphereHitbox* other, Object* oth)
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();
	oT.position += oth->phys.move;
	t.position += th->phys.move;

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

bool CubeHitbox::HitDetect(Object* th, CapsuleHitbox* other, Object* oth)
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
void CubeHitbox::SetTransform(Transform t)
{
}
void CubeHitbox::Draw(Shader* shdr, glm::mat4 p)
{

}
Transform CubeHitbox::GetTransform()
{
	return Transform();
}
#pragma endregion

#pragma region Shpere Collisions
Mesh* SphereHitbox::node_me = nullptr;
Material* SphereHitbox::node_ma = nullptr;
/********************************************************************
*						SPHERE COLLISION
********************************************************************/

void SphereHitbox::init()
{
	node_me = new Mesh("Sphere.obj");
	node_ma = new Material("default-texture.png", "default-normal.png");
}

void SphereHitbox::Draw(Shader* shdr, glm::mat4 p)
{
	shdr->Use();
	shdr->SetI("material.diffuse", 0);
	shdr->SetI("material.normal", 1);
	shdr->SetI("material.specular", 2);

	transform.scale = glm::vec3(radius) * transform.scale;
	glm::mat4 model = p * transform.GetWorldTransform();// *glm::translate(glm::mat4(1.0f), glm::vec3(transform.position.x, transform.position.y, transform.position.z));

	//glm::mat4 model2 = parent.GetWorldTransform() * glm::translate(glm::mat4(1.0f), convertVec4(upperBound));

	unsigned int modelLoc = glGetUniformLocation(shdr->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(model)));

	shdr->SetMat3("normMat", normMat);

	shdr->SetF("material.shine", node_ma->shine);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, node_ma->DIFF);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, node_ma->NORM);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, node_ma->SPEC);

	node_me->Draw(shdr);

	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, 0.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
}

void SphereHitbox::SetTransform(Transform t)
{
	transform = t;
}

Transform SphereHitbox::GetTransform()
{
	return transform;
}

bool SphereHitbox::HitDetect(Object* th, CubeHitbox* other, Object* oth)
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();
	oT.position += oth->phys.move;
	t.position += th->phys.move;

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

bool SphereHitbox::HitDetect(Object* th, SphereHitbox* other, Object* oth)
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();
	oT.position += oth->phys.move;
	t.position += th->phys.move;

	float distance = glm::distance(t.position, oT.position);
	float maxDistance = radius * (t.scale.x + t.scale.y + t.scale.z) / 3 + other->GetRadius() * (oT.scale.x + oT.scale.y + oT.scale.z) / 3;
	
	if (distance < maxDistance)
		return true;
	return false;
}

bool SphereHitbox::HitDetect(Object* th, CapsuleHitbox* other, Object* oth)
{
	return other->HitDetect(oth, this, th);
}

#pragma endregion




#pragma region Capsule Collisions
Mesh* CapsuleHitbox::node_me = nullptr;
Material* CapsuleHitbox::node_ma = nullptr;

void CapsuleHitbox::init()
{
	node_me = new Mesh("Sphere.obj");
	node_ma = new Material("default-texture.png", "default-normal.png");
}

void CapsuleHitbox::SetTransform(Transform t)
{
}

Transform CapsuleHitbox::GetTransform()
{
	return Transform();
}

glm::vec3 CapsuleHitbox::convertVec4(glm::vec4 _vec4)
{
	glm::vec3 pass{ _vec4.x,_vec4.y,_vec4.z };

	return pass;
}

void CapsuleHitbox::Draw(Shader* shdr, glm::mat4 p)
 {
	shdr->Use();
	shdr->SetI("material.diffuse", 0);
	shdr->SetI("material.normal", 1);
	shdr->SetI("material.specular", 2);
	transform.scale = glm::vec3(radius);//
	glm::mat4 model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f),lowerBound);
	


	unsigned int modelLoc = glGetUniformLocation(shdr->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(model)));

	shdr->SetMat3("normMat", normMat);

	shdr->SetF("material.shine", node_ma->shine);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, node_ma->DIFF);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, node_ma->NORM);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, node_ma->SPEC);

	//Center Bottom
	
	node_me->Draw(shdr);

	//Center Top
	model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f),upperBound);
	

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	node_me->Draw(shdr);

	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(upperBound.x + radius, upperBound.y, upperBound.z));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(upperBound.x - radius, upperBound.y, upperBound.z));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(upperBound.x, upperBound.y, upperBound.z + radius));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
	//
	//model = p * transform.GetWorldTransform() * glm::translate(glm::mat4(1.0f), glm::vec3(upperBound.x, upperBound.y, upperBound.z - radius));
	//glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	//
	//node_me->Draw(shdr);
}

bool CapsuleHitbox::HitDetect(Object* th, CapsuleHitbox* other, Object* oth)
 {

	glm::mat4 t = th->getParentTransform();
	glm::mat4 oT = oth->getParentTransform(glm::translate(glm::mat4(1.0f), oth->phys.move));
	//oT = oT * glm::translate(glm::mat4(1.0f), oth->phys.move);

	//Adding an indiscriminant value to 1 so we don't get destroyed by dividing by zero.
	//if (this->height == 1.0f) {
	//	height += 0.000001f;
	//}
	//
	//if (other->height == 1.0f) {
	//	other->height += 0.000001f;
	//}

	glm::vec3 tub = convertVec4(t * transform.GetWorldTransform() * glm::vec4(upperBound.x, upperBound.y, upperBound.z, 1.0f));
	glm::vec3 tlb = convertVec4(t * transform.GetWorldTransform() * glm::vec4(lowerBound.x, lowerBound.y, lowerBound.z, 1.0f));

	glm::vec3 oub = convertVec4(oT * other->transform.GetWorldTransform() * glm::vec4(other->upperBound.x, other->upperBound.y, other->upperBound.z, 1.0f));
	glm::vec3 olb = convertVec4(oT * other->transform.GetWorldTransform() * glm::vec4(other->lowerBound.x, other->lowerBound.y, other->lowerBound.z, 1.0f));

	//glm::vec3 closestPointA;
	//glm::vec3 closestPointB;

	float totalDist; //Calculated later
	float totalRadius = this->radius + other->GetRadius();

	//Find closest point on both lines -- Broken as of 2/1/2020
	{
		//glm::vec3 LineA = tub - tlb;
		//glm::vec3 LineB = oub - olb;

		//glm::vec3 PA1 = tlb;
		//glm::vec3 PA2 = tub;

		//glm::vec3 PB1 = olb;
		//glm::vec3 PB2 = oub;

		//glm::vec3 DirA = glm::normalize(LineA);
		//glm::vec3 DirB = glm::normalize(LineB);

		////Getting a scalar value for length along line B
		//float scalarB = (((glm::dot(LineA,(PB1 - PA1)) * glm::dot(LineA, LineB) / glm::dot(LineA, LineA)) - (glm::dot(LineB, (PB1 - PA1)))	) / (glm::dot(LineB, LineB) - 1));

		//scalarB = glm::clamp(scalarB, -1.0f, 1.0f);

		////Getting a scalar value for length along line A subbing in t.
		//float scalarA = (glm::dot(LineB, (PB1 - PA1)) + glm::dot(LineB, LineB) * scalarB) / glm::dot(LineA, LineB);

		//scalarA = glm::clamp(scalarA, -1.0f, 1.0f);

		////std::cout << scalarB << std::endl;
		////std::cout << scalarA << std::endl;

		//closestPointA = PA1 + ((PA2-PA1) * scalarA);
		//closestPointB = PB1 + ((PB2-PB1) * scalarB);

		////closestPointA = PA1 + ((DirA) * scalarA);
		////closestPointB = PB1 + ((DirB) * scalarB);


	}

	//Calculate Line Segment distance
	{
		constexpr auto SMALL_VALUE = 0.00000001;;

		glm::vec3 LineA = tub - tlb; //u
		glm::vec3 LineB = oub - olb; //v
		glm::vec3 LineC = tlb - olb; //w

		float a = glm::dot(LineA, LineA); //Always >= 0
		float b = glm::dot(LineA,LineB); 
		float c = glm::dot(LineB, LineB); //Always >= 0

		float d = glm::dot(LineA,LineC);
		float e = glm::dot(LineB, LineC);

		float f = (a * c) - (b * b); //Always >= 0	 // D

		float sc, sN, sD = f; // sc = sN / sD, by default sD = D >= 0
		float tc, tN, tD = f; // tc = tN / tD, by default tD = D >= 0


		//The following section is to calculate against certain cases where values may go out of real ranges.
		//Due to the nature of Dot Product in 3D space and in certain orientations.
		
		//Now compute the line parameters of the two closest Points.
		if (f < SMALL_VALUE) { //This means the line is almost parallel
			sN = 0.0f;		   // force using lower bound for line 1
			sD = 1.0f;		   //To prevent a future division by 0.
			tN = e;
			tD = c;
		}
		else//				Get the closest points on the infinite lines.
		{
			sN = (b*e - c*d);
			tN = (a*e - b*d);
			if (sN < 0.0f) {	//sc < 0 , the s=0  edge is visible
				sN = 0.0f;
				tN = e;
				tD = c;
			}
			else if (sN > sD) // sc > 1 , the s=1 edge is visible.
			{
				sN = sD;
				tN = e + b;
				tD = c;
			}
		}

		if (tN < 0.0f) { //tc < 0 , the t = 0 edge is visible.
			tN = 0.0f;

			//recompute sc for this edge
			if (-d < 0.0f) {
				sN = 0.0f;
			}
			else if (-d > a) {
				sN = sD;
			}
			else {
				sN = -d;
				sD = a;
			}	
		}
		else if (tN > tD) { //tc > 1 , the t = 1 edge is visible.
			tN = tD;

			//recompute sc for this edge.
			if ((-d + b) < 0.0f) {
				sN = 0;
			}
			else if ((-d + b) > a)
			{
				sN = sD;
			}
			else
			{
				sN = (-d + b);
				sD = a;
			}
		}

		//do division to calculate sc and tc
		sc = (glm::abs(sN) < SMALL_VALUE ? 0.0 : (sN / sD));
		tc = (glm::abs(tN) < SMALL_VALUE ? 0.0 : (tN / tD));
					
		//We get the closest vector that is between both lines so we need it's length
		//sc and tc are the parameters for vector lines A and B (multiplier to be a point along them)

		totalDist = glm::length(LineC + (sc * LineA) - (tc * LineB));

	}

	//float difference = totalDist - totalRadius;
	//
	//if (difference < 0.00000599f && difference > -0.00000599f) {
	//	return true;
	//}
	//else
	if (totalDist < totalRadius)
		return true;
	else
		return false;
}

bool CapsuleHitbox::HitDetect(Object* th, CubeHitbox* other, Object* oth)
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();
	oT.position += oth->phys.move;
	t.position += th->phys.move;

	return false;
}

glm::mat3 ConvertFromMat4(glm::mat4 km) {
	return {km[0].x, km[0].y, km[0].z, km[1].x, km[1].y, km[1].z, km[2].x, km[2].y, km[2].z };
}

bool CapsuleHitbox::HitDetect(Object* th, SphereHitbox* other, Object* oth)
{
	Transform t = th->GetTransform();
	Transform oT = oth->GetTransform();

	oT.position += oth->phys.move; //Sphere Position
	t.position += th->phys.move; //Capsule Position

	glm::vec3 tub = convertVec4(t.GetWorldTransform() * transform.GetWorldTransform() * glm::vec4(upperBound.x, upperBound.y, upperBound.z, 1.0f));
	glm::vec3 tlb = convertVec4(t.GetWorldTransform() * transform.GetWorldTransform() * glm::vec4(lowerBound.x, lowerBound.y, lowerBound.z, 1.0f));

	glm::vec3 op = convertVec4(oT.GetWorldTransform() * other->transform.GetWorldTransform() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

	glm::vec3 cpa; //closest point on the line.

	{
		

		glm::vec3 v = tub - tlb; //Upper - lower,  AKA p2 - p1
		glm::vec3 w = op - tlb; // point - lower, P - p1;

		float c1 = glm::dot(w,v);
		float c2 = glm::dot(v, v);

		if (c1 <= 0) {  //return lb
			cpa = tlb;
		}
		else if (c2 <= c1) {  //return ub
			cpa = tub;
		}
		else
		{
			float b = c1 / c2; // this gives some value 0 - 1 ( I assume )

			glm::vec3 temp = tlb + (b * tub); //This is essentially lerp along the line for closest point.
			cpa = temp;
		}

	}

	// ******* Depreciated as of 3/15/2020
	//float scalarA = (-1 * glm::dot(tlb - op, tub - tlb)) / glm::pow(glm::distance(tub, tlb), 2);
	//
	//glm::vec3 cpa = tlb + (tub - tlb) * scalarA;



	//Sphere stuff below
	glm::vec3 dir = cpa - op;

	glm::mat3 invertRotScale = glm::inverse(ConvertFromMat4(oT.GetWorldTransform() * other->transform.GetWorldTransform()));

	glm::vec3 neutDir = glm::normalize(invertRotScale * dir);
	
	float or = glm::sqrt(glm::pow(neutDir.x * other->transform.scale.x, 2) +
		glm::pow(neutDir.y * other->transform.scale.y, 2) +
		glm::pow(neutDir.z * other->transform.scale.z, 2));

	float max_dist = or + radius;
	float currentDist = glm::length(cpa, op);

	if (glm::distance(cpa, op) <= max_dist)
		return true;
	else
		return false;
}


#pragma endregion

void Hitbox::parentTransform(Transform t)
{
	parent = t;
}
