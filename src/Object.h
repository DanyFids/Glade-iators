#pragma once
#include<GLM/glm.hpp>
#include<GLM/gtc/matrix_transform.hpp>
#include<GLM/gtc/quaternion.hpp>
#include<vector>
#include <cmath>
#include <string>
//#include <chrono>

class Camera;
class Material;
class Mesh;
class Shader;
class Hitbox;
class Joint;
class SkelMesh;

struct Transform {
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);

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

	glm::mat4 GetBoneTransform() const {
		glm::quat qPitch = glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat qYaw = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qRoll = glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::quat rot = qYaw * qPitch * qRoll;

		return
			glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f) * glm::length(position)) *
			glm::mat4_cast(rot) *
			glm::scale(glm::mat4(1.0f), scale)
			;
	}

	glm::mat4 GetRotEul() const {
		glm::quat qPitch = glm::angleAxis(glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::quat qYaw = glm::angleAxis(glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::quat qRoll = glm::angleAxis(glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::quat rot = qYaw * qPitch * qRoll;

		return glm::mat4_cast(rot);
	}

	glm::mat4 GetRotQuat() const {
		return glm::mat4_cast(glm::quat(glm::radians(rotation)));
	}

	glm::mat4 GetRotScale() const {
		return
			glm::mat4_cast(glm::quat(glm::radians(rotation))) *
			glm::scale(glm::mat4(1.0f), scale);
	}

	glm::mat4 GetRot() const {
		return glm::mat4_cast(glm::quat(glm::radians(rotation)));
	}

	glm::mat4 GetQuatTransform() const {
		return
			glm::translate(glm::mat4(1.0f), position) *
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
	std::vector <Object*>children;
	Joint* parent_joint = nullptr;
	SkelMesh* parent_Mesh = nullptr;

public:
	Hitbox* hitbox;
	Object* parent = nullptr;
	PhysicsBody phys;

	Object();
	Object(Mesh* me, Material* ma, Hitbox* hb);
	Object(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, Joint* p = nullptr, SkelMesh* m = nullptr);

	glm::mat4 GetTrueTransform();
	virtual void Update(float dt);
	virtual void Draw(Shader* shader, std::vector<Camera*> cam, Shader* childShader = nullptr);
	virtual void DrawChild(Shader* shader, glm::mat4 parent);
	virtual void DestroyChild(int c);
	void Rotate(glm::vec3 rot);
	void Rotate(float tht, glm::vec3 dir);
	void Move(glm::vec3 dir);
	void Scale(glm::vec3 scl);
	void SetPosition(glm::vec3 pos);
	void SetRotation(glm::vec3 rot);
	void addChild(Object* child);

	glm::mat4 getParentTransform();

	virtual bool HitDetect(Object* other);

	virtual void ApplyMove();

	Joint* GetParentJoint() { return parent_joint; }
	glm::mat4 TransformTo();

	glm::vec3 GetPosition() { return transform.position; };
	Transform GetTransform() { return transform; }

	Mesh* GetMesh() { return mesh; }

	friend class Hitbox;
};

enum PLAYER_STATE {
	idle, walking, attacking, blocking, rolling, taunting
};

class Weapon;

class Player : public Object {
	static const float STAM_DECAY;
	static const float STAM_RECOV;
	static const float RECOV_TIME;

	bool run = false;
	bool anim_lock = false;
	float health;
	float stamina;
	float recov_timer = 0.0f;

	SkelMesh* _mesh;

	PLAYER_STATE state;

	Weapon* weapon;

	glm::vec3 last_root_pos = glm::vec3(0.0f);

public:
	static const float MAX_HEALTH;
	static const float MAX_STAMINA;

	Player();
	Player(SkelMesh* me, Material* ma, Hitbox* hb);
	Player(SkelMesh* me, Material* ma, Hitbox* hb, glm::vec3 pos);

	virtual void Update(float dt);
	virtual bool HitDetect(Object* other);

	float GetHP() { return health; }
	void dmgHP(float _dmg) { health -= _dmg; }
	void dmgSTAM(float _dmg) { stamina -= _dmg; }
	float GetStam() { return stamina; }
	bool CanRun() { return recov_timer <= 0.0f; }

	void PlayAnim(std::string n, unsigned int c = 0, float i = 1.0f, float s = 1.0f);

	void Run();
	void StopRun();

	void Roll();
	void Idle();
	void Attack();

	PLAYER_STATE GetState() {return state;}
	void SetState(PLAYER_STATE s) {state = s;}
	bool IsLocked() { return anim_lock; }
};

class Weapon : public Object {
private:
	float damage, stamina_cost;

	std::vector<std::string> attack_anims;
public:
	Weapon(Mesh* me, Material* ma, Hitbox* hb, std::vector<std::string> atks, float dmg, float stam);

	std::string GetAtkAnim(unsigned int c_id = 0);
	float GetDamage() {return damage;}
	float GetStaminaCost() { return stamina_cost; }

	virtual bool HitDetect(Object* other);
};


class Attack : public Object
{
private:

public:
	virtual void Update(float dt);
	virtual bool HitDetect(Player* other);

	//static Object* ABox;
	int player;
	float time;
	bool Hit = false;

	Attack(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, unsigned int P);

	int getPlayer() { return player; }
	//void init();
};

class Shield : public Object
{
private:

public:
	virtual void Update(float dt);
	virtual bool HitDetect(Object* other);

	//static Object* ABox;
	int player;

	Shield(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, unsigned int P);

	//void init();
};

class SplineMan : public Object
{
public:
	virtual void Update(float dt);
	virtual bool HitDetect(Object* other);

	//float StartT;
	float CurrT = 0;
	float MaxT = 0.2f;
	int current = 0;
	bool init;
	int direction = 1;
	std::vector<glm::vec3> pathPos;

	void CatmullBetweenPoints(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);

	SplineMan(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, std::vector<glm::vec3> wasd);


};