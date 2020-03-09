#include "UI.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include<GLM/gtc/matrix_transform.hpp>
#include "Constants.h"
#include "Object.h"
#include"Lerp.h"

float quad_prim[] = {
	// x, y, z, r, g, b, u, v
	0.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
	 1.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
	 1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
	0.0f,  1.0f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f
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

UI::UI(int width, int height, glm::vec3 pos, Material* ma)
{
	material = ma;

	this->pos = pos;
	this->dim = glm::vec2(width, height);

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(width, height, 1.0f));
}

int UI::scaleX(int _x)
{
	scale.x = _x;
	return scale.x;
}

void UI::Resize(int x, int y)
{
	dim.x = x;
	dim.y = y;
}

void UI::ChangeTex(Material* mat)
{
	material = mat;
}

void UI::Draw(glm::vec2 scrn) {
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(pos.x, pos.y, 0.0f));
	model = glm::scale(model, glm::vec3(dim.x, dim.y, 1.0f));
	
	SHADER->Use();

	SHADER->SetMat4("proj", projection);
	SHADER->SetMat4("model", model);
	SHADER->SetF("opacity", opacity);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, material->DIFF);
	SHADER->SetI("tex", 0);

	QUAD->Draw(SHADER);
}

Mesh* UI::QUAD = nullptr;
Shader* UI::SHADER = nullptr;
glm::mat4 UI::projection = glm::mat4(1.0f);

const int StaminaBar::MAX_WIDTH = 250;
const int StaminaBar::HEIGHT = 5;

StaminaBar::StaminaBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg): UI(MAX_WIDTH, HEIGHT, glm::vec3(pos, 0), ma)
{
	player = p;
	bg = _bg;
}

void StaminaBar::Update(float dt)
{
	float perc = player->GetStam() / Player::MAX_STAMINA;

	dim.x = (float)MAX_WIDTH * perc;
}

void StaminaBar::Draw(glm::vec2 scrn)
{
	bg->Draw(scrn);

	UI::Draw(scrn);
}

const int HealthBar::MAX_WIDTH = 250;
const int HealthBar::HEIGHT = 5;

HealthBar::HealthBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg) : UI(MAX_WIDTH, HEIGHT, glm::vec3(pos, 0), ma)
{
	player = p;
	bg = _bg;

}

void HealthBar::Update(float dt)
{
	if (cur != (player->GetHP() / Player::MAX_HEALTH)) {

		if (!hpLerp || perc != player->GetHP() / Player::MAX_HEALTH) {
			hpLerp = true;
			hpTime = MAX_HPTIME;
			temp = perc;
			perc = player->GetHP() / Player::MAX_HEALTH;
		}

		cur = lerp(perc, temp, hpTime / MAX_HPTIME);

		dim.x = (float)MAX_WIDTH * cur;
	}

	if (hpTime > 0) {
		hpTime -= dt;
	}
	else {
		hpLerp = false;
	}

	//lerp(green, ylw, time / MAX_TIME);

	/*
	
	plan:
	get og value
	get new value
	lerp between using time
	
	*/

	if (time < MAX_TIME) {
		time += dt;
	}

}

void HealthBar::Draw(glm::vec2 scrn)
{
	bg->Draw(scrn);
	glm::vec3 green(0.0f, 1.0f, 0.0f);
	glm::vec3 ylw(1.0f, 1.0f, 0.0f);
	glm::vec3 orange(1.0f, 0.65f, 0.0f);
	glm::vec3 red(1.0f, 0.0f, 0.0f);
	glm::vec3 brown(0.3f, 0.2f, 0.0f);

	/*
	green 100-81
	yellow 80-61
	orange 60-41
	red 40-21
	brown 20-0
	glm::vec3 color = lerp(ylw, orange, player->GetHP() / player->MAX_HEALTH);
	*/

	if (player->GetHP() <= 100 && player->GetHP() > 80) {
		glm::vec3 color = green;
		GREEN = true;
		UI::SHADER->SetVec3("indexColor", color);
	}
	else if (player->GetHP() < 81 && player->GetHP() > 60) {
		if (!YELLOW) {
			glm::vec3 color = lerp(green, ylw, time / MAX_TIME);
			UI::SHADER->SetVec3("indexColor", color);
		}
		else {
			glm::vec3 color = ylw;
			UI::SHADER->SetVec3("indexColor", color);
		}

		if (!LERPING && !YELLOW) {
			time = 0.0f;
			LERPING = true;
		}

		if (time >= MAX_TIME) {
			YELLOW = true;
			LERPING = false;
		}
	}
	else if (player->GetHP() < 61 && player->GetHP() > 40) {
		if (!ORANGE) {
			glm::vec3 color = lerp(ylw, orange, time / MAX_TIME);
			UI::SHADER->SetVec3("indexColor", color);
		}
		else {
			glm::vec3 color = orange;
			UI::SHADER->SetVec3("indexColor", color);
		}

		if (!LERPING && !ORANGE) {
			time = 0.0f;
			LERPING = true;
		}

		if (time >= MAX_TIME) {
			ORANGE = true;
			LERPING = false;
		}
	}
	else if (player->GetHP() < 41 && player->GetHP() > 20) {
		if (!RED) {
			glm::vec3 color = lerp(orange, red, time / MAX_TIME);
			UI::SHADER->SetVec3("indexColor", color);
		}
		else {
			glm::vec3 color = red;
			UI::SHADER->SetVec3("indexColor", color);
		}

		if (!LERPING && !RED) {
			time = 0.0f;
			LERPING = true;
		}

		if (time >= MAX_TIME) {
			RED = true;
			LERPING = false;
		}
	}
	else if (player->GetHP() < 21 && player->GetHP() >= 0) {
		if (!BROWN) {
			glm::vec3 color = lerp(red, brown, time / MAX_TIME);
			UI::SHADER->SetVec3("indexColor", color);
		}
		else {
			glm::vec3 color = brown;
			UI::SHADER->SetVec3("indexColor", color);
		}

		if (!LERPING && !BROWN) {
			time = 0.0f;
			LERPING = true;
		}

		if (time >= MAX_TIME) {
			BROWN = true;
			LERPING = false;
		}
	}

	UI::Draw(scrn);
}

const int CrowdBar::MAX_WIDTH = 180;
const int CrowdBar::HEIGHT = 20;
CrowdBar::CrowdBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg) : UI(MAX_WIDTH, HEIGHT, glm::vec3(pos, 0), ma)
{
	player = p;
	bg = _bg;
}

void CrowdBar::Update(float dt)
{
	float perc = 1.0f;

	dim.x = (float)MAX_WIDTH * perc;
	 
	time -= dt;
}

void CrowdBar::Draw(glm::vec2 scrn)
{
	bg->Draw(scrn);

	UI::Draw(scrn);
}

const int Button::HEIGHT = 70;
const int Button::MAX_WIDTH = 200;
Button::Button(glm::vec2 pos, Material* ma) : UI(MAX_WIDTH, HEIGHT, glm::vec3(pos, 0), ma)
{
}

//Button::Button(int width, int height, glm::vec2 pos, Material* ma) : UI(width, height, glm::vec3(pos, 0), ma)
//{
//}

void Button::Update(float dt)
{
}

void Button::Draw(glm::vec2 scrn)
{
	UI::Draw(scrn);
}

const int ButtonSelect::HEIGHT = 80;
const int ButtonSelect::MAX_WIDTH = 210;
ButtonSelect::ButtonSelect(int _player, glm::vec2 _pos, Material* ma) : UI(MAX_WIDTH, HEIGHT, glm::vec3(_pos, 0), ma)
{
	player = _player;
	opacity = 0.6;
}

//ButtonSelect::ButtonSelect(int width, int height, int _player, glm::vec2 pos, Material* ma) : UI(width, height, glm::vec3(pos, 0), ma)
//{
//	player = _player;
//	opacity = 0.6;
//}

void ButtonSelect::move(int _x, int _y)
{
	pos.x += _x;
	pos.y += _y;
}

void ButtonSelect::Update(float dt)
{

}

void ButtonSelect::Draw(glm::vec2 scrn)
{
	UI::Draw(scrn);
}