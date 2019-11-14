#include "UI.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include<GLM/gtc/matrix_transform.hpp>
#include "Constants.h"
#include "Object.h"

float quad_prim[] = {
	// x, y, z, r, g, b, u, v
	0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,
	 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,
	 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,
	0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f
};

unsigned int quad_index[] = {
	0, 1, 2,
	0, 2, 3
};

void UI::INIT()
{
	SHADER = new Shader("Shaders/UI_shader.vert", "Shaders/UI_shader.frag");
	QUAD = new Mesh(quad_prim, 4, quad_index, 6);
	projection = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT, -1.f, 1.f);
}

UI::UI(int width, int height, glm::vec2 pos, Material* ma)
{
	material = ma;

	this->pos = pos;
	this->dim = glm::vec2(width, height);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(width, height, 1.0f));
}

void UI::Draw(glm::vec2 scrn) {
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(dim.x, dim.y, 1.0f));
	
	SHADER->Use();

	SHADER->SetMat4("proj", projection);
	SHADER->SetMat4("model", model);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->DIFF);
	SHADER->SetI("tex", 0);

	QUAD->Draw(SHADER);
}

Mesh* UI::QUAD = nullptr;
Shader* UI::SHADER = nullptr;
glm::mat4 UI::projection = glm::mat4(1.0f);

const int StaminaBar::MAX_WIDTH = 150;
const int StaminaBar::HEIGHT = 20;

StaminaBar::StaminaBar(Player* p, glm::vec2 pos, Material* ma): UI(MAX_WIDTH, HEIGHT, pos, ma)
{
	player = p;
}

void StaminaBar::Update(float dt)
{
	float perc = player->GetStam() / Player::MAX_STAMINA;

	dim.x = (float)MAX_WIDTH * perc;
}
