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
	virtual bool HitDetect(Object* th, CubeHitbox * other, Object* oth) = 0;
	virtual bool HitDetect(Object* th, SphereHitbox* other, Object* oth) = 0;
	virtual bool HitDetect(Object* th, CapsuleHitbox* other, Object* oth) = 0;
	virtual void parentTransform(Transform t);
	virtual void Draw(Shader* shdr, glm::mat4 p) = 0;
	virtual Transform GetTransform() = 0;
	virtual void SetTransform(Transform t) = 0;

	void SetPosition(glm::vec3 pos) { transform.position = pos; }
	void SetRotation(glm::vec3 rot) { transform.rotation = rot; }
	void SetScale(glm::vec3 scl) { transform.position = scl; }
protected:
	Transform parent;
	Transform transform;
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

	virtual bool HitDetect(Object* th, CubeHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, SphereHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, CapsuleHitbox* other, Object* oth);
	virtual bool HitDetectSAT(Transform t, CubeHitbox* other, Transform oT);

	virtual void setVerts(Transform t);
	virtual void setNorms();
	virtual void setEdgeDirs();
	virtual glm::vec3 convertVec4(glm::vec4 _vec4);
	
	virtual void GetInterval(CubeHitbox* o, glm::vec3 axis, float& min, float& max);

	virtual bool testIntersection(Transform t, CubeHitbox* object2, Transform oT, glm::vec3 offset);
};

class CapsuleHitbox;
class SphereHitbox : public Hitbox {
	float radius;
	static Mesh* node_me;
	static Material* node_ma;
public:
	SphereHitbox(float r) :radius(r) {};
	static void init();
	virtual Transform GetTransform() override;

	virtual void Draw(Shader* shdr, glm::mat4 p) override;
	float GetRadius() { return radius; }

	virtual bool HitDetect(Object* th, CubeHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, SphereHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, CapsuleHitbox* other, Object* oth);

	friend class CapsuleHitbox;
};

class CapsuleHitbox : public Hitbox {
	static Mesh* node_me;
	static Material* node_ma;
	
	float radius;
	float height;
	glm::vec3 upperBound, lowerBound;
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
	virtual void Draw( Shader* shdr, Transform p) override;
	virtual bool HitDetect(Object* th, CubeHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, SphereHitbox* other, Object* oth);
	virtual bool HitDetect(Object* th, CapsuleHitbox* other, Object* oth);
	
};