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
