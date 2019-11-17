#pragma once
#include<GLM/glm.hpp>
#include<GLM/gtc/matrix_transform.hpp>
#include<GLM/gtc/quaternion.hpp>
#include<vector>

class Camera;
class Material;
class Mesh;
class Shader;
class Hitbox;

struct Transform {
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;

	glm::mat4 GetWorldTransform() const {
		glm::quat qPitch = glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat qYaw = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qRoll = glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::quat rot = qYaw * qPitch * qRoll;

		return
			glm::translate(glm::mat4(1.0f), position) *
			glm::mat4_cast(rot) *
			glm::scale(glm::mat4(1.0f), scale)
			;
	}

	glm::mat4 GetRotScale() const {
		return
			glm::mat4_cast(glm::quat(glm::radians(rotation))) *
			glm::scale(glm::mat4(1.0f), scale);
	}
};

struct PhysicsBody {
	glm::vec3 move = glm::vec3(0.0f, 0.0f, 0.0f);
};

class Object {
protected:
	Mesh* mesh;
	Material* material;
	Transform transform;

public:
	Hitbox* hitbox;
	PhysicsBody phys;

	Object();
	Object(Mesh* me, Material* ma, Hitbox* hb);
	Object(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos);

	virtual void Update(float dt);
	virtual void Draw(Shader* shader, std::vector<Camera*> cam);
	void Rotate(glm::vec3 rot);
	void Rotate(float tht, glm::vec3 dir);
	void Move(glm::vec3 dir);
	void Scale(glm::vec3 scl);
	void SetPosition(glm::vec3 pos);

	virtual bool HitDetect(Object* other);

	virtual void ApplyMove();

	glm::vec3 GetPosition() { return transform.position; };
	Transform GetTransform() { return transform; }
};

class Player : public Object {
	static const float STAM_DECAY;
	static const float STAM_RECOV;
	static const float RECOV_TIME;

	bool run = false;
	float stamina;
	float recov_timer = 0.0f;

public:
	static const float MAX_STAMINA;

	Player(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos);

	virtual void Update(float dt);
	virtual bool HitDetect(Object* other);

	float GetStam() { return stamina; }
	bool CanRun() { return recov_timer <= 0.0f; }

	void Run();
	void StopRun();
};