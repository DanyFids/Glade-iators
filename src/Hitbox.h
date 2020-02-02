#pragma once
#include<GLM/glm.hpp>
#include<vector>
#include "Mesh.h"
#include "Texture.h"
#include "Object.h"

struct Transform;

class CubeHitbox;
class SphereHitbox;
class CapsuleHitbox;

class Hitbox {
	
public:
	virtual bool HitDetect(Transform t, CubeHitbox * other, Transform oT) = 0;
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT) = 0;
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT) = 0;
	virtual void parentTransform(Transform t);
	virtual void Draw(Shader* shdr, glm::mat4 p) = 0;
	virtual Transform GetTransform() = 0;
	virtual void SetTransform(Transform t) = 0;
protected:
	Transform parent;
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
	virtual void SetTransform(Transform t) override;
	virtual void Draw(Shader* shdr, glm::mat4 p) override;

	virtual Transform GetTransform() override;

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
	Transform transform;
	static Mesh* node_me;
	static Material* node_ma;
public:
	SphereHitbox(float r) :radius(r) {};
	static void init();
	virtual Transform GetTransform() override;

	virtual void Draw(Shader* shdr, glm::mat4 p) override;
	float GetRadius() { return radius; }

	virtual void SetTransform(Transform t) override;

	virtual bool HitDetect(Transform t, CubeHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT);
};

class CapsuleHitbox : public Hitbox {
	static Mesh* node_me;
	static Material* node_ma;
	
	float radius;
	float height;
	glm::vec3 upperBound, lowerBound;
	Transform transform;
public:
	static void init();

	CapsuleHitbox(float r, float h) :radius(r), height(h) {
		lowerBound = glm::vec3(0.0f, 0.0f, 0.0f);
		upperBound = glm::vec3(0.0f, h, 0.0f);
	};
	
	float GetRadius() { return radius; }
	virtual void SetTransform(Transform t) override;
	virtual Transform GetTransform() override;

	virtual glm::vec3 convertVec4(glm::vec4 _vec4);
	virtual void Draw( Shader* shdr, glm::mat4 p);
	virtual bool HitDetect(Transform t, CapsuleHitbox* other, Transform oT);//Capsule to capsule
	virtual bool HitDetect(Transform t, CubeHitbox* other, Transform oT);
	virtual bool HitDetect(Transform t, SphereHitbox* other, Transform oT);
	
};