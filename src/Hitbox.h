#pragma once
#include<GLM/glm.hpp>
#include<vector>

struct Transform;

class CubeHitbox;
class SphereHitbox;
class CapsuleHitbox;

class Hitbox {

public:
	virtual bool HitDetect(Transform t, CubeHitbox * other, Transform oT) = 0;
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT) = 0;
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT) = 0;
};

class CubeHitbox : public Hitbox {
	glm::vec3 dim;
	

	std::vector<glm::vec4> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> edgeDirs;

	//Temporary min-max ranges for SAT collision detection
	float min;
	float max;
public:
	CubeHitbox(float w, float h, float d);

	glm::vec3 GetDim() {return dim;}


	virtual bool HitDetect(Transform t, CubeHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT);
	virtual bool HitDetectSAT(Transform t, CubeHitbox* other, Transform oT);

	virtual void setVerts(Transform t);
	virtual void setNorms();
	virtual void setEdgeDirs();
	virtual glm::vec3 convertVec4(glm::vec4 _vec4);
	
	virtual void GetInterval(CubeHitbox* o, glm::vec3 axis, float& min, float& max);

	virtual bool testIntersection(Transform t, CubeHitbox* object2, Transform oT, glm::vec3 offset);
};

class SphereHitbox : public Hitbox {
	float radius;
public:
	SphereHitbox(float r) :radius(r) {};

	float GetRadius() { return radius; }

	virtual bool HitDetect(Transform t, CubeHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT);
};

class CapsuleHitbox : public Hitbox {

	float radius;
	float height;
	glm::vec4 upperBound, lowerBound;
public:
	CapsuleHitbox(float r, float h) :radius(r), height(h) {
	
	
	};
	
	float GetRadius() { return radius; }

	virtual glm::vec3 convertVec4(glm::vec4 _vec4);

	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT);//Capsule to capsule
	virtual bool HitDetect(Transform t, CubeHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT);
};