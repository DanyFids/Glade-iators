#include "Object.h"
#include<GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include"Mesh.h"
#include"Shader.h"
#include"Texture.h"

Object::Object()
{
	mesh = nullptr;
	material = nullptr;
	position = glm::vec3();
	scale = glm::vec3(1.0f);
	rotation = glm::mat4(1.0f);
}

Object::Object(Mesh* me, Material* ma)
{
	mesh = me;
	material = ma;
	position = glm::vec3();
	scale = glm::vec3(1.0f);
	rotation = glm::mat4(1.0f);
}

Object::Object(Mesh* me, Material* ma, glm::vec3 pos)
{
	mesh = me;
	material = ma;
	position = pos;
	scale = glm::vec3(1.0f);
	rotation = glm::mat4(1.0f);
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

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, position);
	model = glm::scale(model, scale);
	model = model * rotation;

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

	mesh->Draw(shader, cams);
}

void Object::Rotate(float tht, glm::vec3 dir)
{
	rotation = glm::rotate(rotation, glm::radians(tht), dir);
}

void Object::Move(glm::vec3 dir)
{
	position += dir;
}

void Object::Scale(glm::vec3 scl)
{
	scale = scl;
}
