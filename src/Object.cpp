#include "Object.h"
#include<GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include"Mesh.h"
#include"Shader.h"
#include"Texture.h"
#include"Hitbox.h"
#include "UI.h"
#include"Lerp.h"


Object::Object()
{
	mesh = nullptr;
	material = nullptr;
	hitbox = nullptr;
	transform.position = glm::vec3();
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

Object::Object(Mesh* me, Material* ma, Hitbox* hb)
{
	mesh = me;
	material = ma;
	hitbox = hb;
	transform.position = glm::vec3();
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

Object::Object(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos)
{
	mesh = me;
	material = ma;
	hitbox = hb;
	transform.position = pos;
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

void Object::Update(float dt)
{
}

void Object::Draw(Shader* shader, std::vector<Camera*> cams)
{
	shader->Use();
	shader->SetI("material.diffuse", 0);
	shader->SetI("material.normal", 1);
	shader->SetI("material.specular", 2);

	glm::mat4 model = transform.GetWorldTransform();

	unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(model)));

	shader->SetMat3("normMat", normMat);

	shader->SetF("material.shine", material->shine);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->DIFF);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, material->NORM);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, material->SPEC);

	mesh->Draw(shader);
}

void Object::Rotate(glm::vec3 rot) {
	transform.rotation += rot;
}

void Object::Move(glm::vec3 dir)
{
	transform.position += dir;
}

void Object::Scale(glm::vec3 scl)
{
	transform.scale = scl;
}

void Object::SetPosition(glm::vec3 pos)
{
	transform.position = pos;
}

bool Object::HitDetect(Object* other)
{
	return false;
}

void Object::ApplyMove() {
	transform.position += phys.move;
	phys.move = glm::vec3(0.0f, 0.0f, 0.0f);
}

const float Player::MAX_HEALTH = 100.0f;
const float Player::MAX_STAMINA = 100.0f;
const float Player::STAM_DECAY = 30.0f;
const float Player::STAM_RECOV = 20.0f;
const float Player::RECOV_TIME = 2.0f;

Player::Player() {
	health = MAX_HEALTH;
	stamina = MAX_STAMINA;

	mesh = nullptr;
	material = nullptr;
	hitbox = nullptr;
	transform.position = glm::vec3();
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

Player::Player(Mesh* me, Material* ma, Hitbox* hb) {
	health = MAX_HEALTH;
	stamina = MAX_STAMINA;

	mesh = me;
	material = ma;
	hitbox = hb;
	transform.position = glm::vec3();
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

Player::Player(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos) : Object(me, ma, hb, pos)
{
	health = MAX_HEALTH;
	stamina = MAX_STAMINA;

	mesh = me;
	material = ma;
	hitbox = hb;
	transform.position = pos;
	transform.scale = glm::vec3(1.0f);
	transform.rotation = glm::vec3();
}

void Player::Update(float dt)
{
	if (recov_timer > 0.0f) {
		recov_timer -= dt;
	}

	if (run) {
		if (stamina > 0.0f && glm::length(phys.move) != 0.0f) {
			phys.move *= 2.0f;
			stamina -= STAM_DECAY * dt;
		}
		else {
			run = false;
			recov_timer = RECOV_TIME;
		}
	}
	else {
		if (stamina < MAX_STAMINA) {
			stamina += STAM_RECOV * dt;

			stamina = glm::min(stamina, MAX_STAMINA); 
		}
	}

	Object::Update(dt);
}

bool Player::HitDetect(Object* other) 
{
	Transform predict = transform;
	predict.position += phys.move;

	if (other->hitbox->HitDetect(other->GetTransform(), (CubeHitbox*)this->hitbox, predict)) { 
		for (float t = 1.0f; t >= -0.1f; t -= 0.1f) {
			t = glm::max(t, 0.0f);
			glm::vec3 fixSpd = lerp(glm::vec3(0.0f, 0.0f, 0.0f), phys.move, t);

			predict = transform;
			predict.position += fixSpd;

			if (!other->hitbox->HitDetect(other->GetTransform(), (CubeHitbox*)this->hitbox, predict)) {
				phys.move = fixSpd;
				break;
			}
		}

		if (run) {
			run = false;
			recov_timer = RECOV_TIME;
		}

		return true;
	}

	return false;
}

void Player::Run()
{
	if (!run && recov_timer <= 0.0f) {
		run = true;
	}
}

void Player::StopRun()
{
	run = false;
}



//Object* Attack::ABox = nullptr;

void Attack::Update(float dt)
{
}

bool Attack::HitDetect(Object* other)
{
	if (other->hitbox->HitDetect(other->GetTransform(), (CubeHitbox*)this->hitbox, this->GetTransform()) && Hit == false) {
		this->Hit = true;
		return true;
	}

	return false;
}

Attack::Attack(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, unsigned int P) : Object(me, ma, hb, pos)
{
	player = P;
	time = 0.5f;
	Hit = false;
}
//
//void Attack::init() {
//	Mesh* Amesh = new Mesh("d6.obj");
//	Material* Amat = new Material("missing_tex.png");
//	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);
//
//	ABox = new Object(Amesh, Amat, basicCubeHB);
//}

void Shield::Update(float dt)
{
}

bool Shield::HitDetect(Object* other)
{
	return false;
}

Shield::Shield(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, unsigned int P) : Object(me, ma, hb, pos)
{
	player = P;
}

void SplineMan::Update(float dt)
{
	CurrT += dt;
	{
		if (init == false)
		{
			CurrT = 0;
			init = true;
		}
		if (direction == 1)
		{
			if (current <= pathPos.size() - 4)
			{
				CatmullBetweenPoints(pathPos[current], pathPos[current + 1], pathPos[current + 2], pathPos[current + 3]);
			}
			else
			{
				direction = 2;
				current = 0;
				init = false;
			}
		}
		else if (direction == 2)
		{
			if (current <= pathPos.size() - 4)
			{
				CatmullBetweenPoints(pathPos[(pathPos.size() - current) - 1], pathPos[(pathPos.size() - current) - 2], pathPos[(pathPos.size() - current) - 3], pathPos[(pathPos.size() - current) - 4]);
			}
			else
			{

				direction = 1;
				current = 0;
				init = false;
			}
		}
		//CatmullBetweenPoints(MarkerList[MarkerListLength - current].position, MarkerList[current + 1].position, MarkerList[current + 2].position, MarkerList[current + 3].position);
	}
}

bool SplineMan::HitDetect(Object* other)
{
	return false;
}

void SplineMan::CatmullBetweenPoints(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
	if (init == false)
	{
		CurrT = 0;
	}
	float x = a.x + b.x;
	float y = a.y + b.y;
	float z = a.z + b.z;
	float squared = pow(x, 2) + pow(y, 2) + pow(z, 2);
	float Length = sqrt(squared);

	float distCovered =  CurrT/MaxT;

	float t = distCovered / Length;

	glm::vec3 q = 2.f * b;
	glm::vec3 w = c - a;
	glm::vec3 e = 2.f * a - 5.f * b + 4.f * c - d;
	glm::vec3 r = -a + 3.f * b - 3.f * c + d;

	//The cubic polynomial: a + b * t + c * t^2 + d * t^3
	glm::vec3 pos = 0.5f * (q + (w * t) + (e * t * t) + (r * t * t * t));

	transform.position = pos;
	if (current == pathPos.size() - 1)
	{
		init = false;
	
		if (direction == 1)
		{
			direction = 2;
		}
		else
		{
			direction = 1;
		}

	}
	else if (t >= 1)
	{
		current++;
		CurrT = 0;
	}
}

SplineMan::SplineMan(Mesh* me, Material* ma, Hitbox* hb, glm::vec3 pos, std::vector<glm::vec3> wasd) : Object(me, ma, hb, pos)
{
	pathPos = wasd;
}