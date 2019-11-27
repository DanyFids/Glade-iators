#pragma once
#include<string>
#include<vector>
#include<GLM/glm.hpp>

#include "Texture.h"

class Mesh;
//class Material;
class Shader;
class Camera;
class Player;

class UI {
protected:
	static Mesh* QUAD;
	static glm::mat4 projection;
	glm::vec3 pos;
	glm::vec2 dim;
	glm::vec2 scale = {1.0f , 1.0f};

	glm::mat4 model;
	Material* material;
	UI* bg = nullptr;

public:
	static Shader* SHADER;

	static void INIT();

	UI(int width, int height, glm::vec3 pos, Material* ma);
	int scaleX(int _x);
	virtual void Draw(glm::vec2 scrn);
	virtual void Update(float dt) {};
};

class StaminaBar : public UI {
	static const int HEIGHT;
	static const int MAX_WIDTH;

	Player* player;

public:
	StaminaBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg);

	virtual void Update(float dt);
	virtual void Draw(glm::vec2 scrn);
};

class HealthBar : public UI {
	static const int HEIGHT;
	static const int MAX_WIDTH;

	float hpTime = 0.0f;
	const float MAX_HPTIME = 0.2f;
	bool hpLerp = false;
	float time = 0.0f;
	const float MAX_TIME = 0.2f;
	bool GREEN = false;
	bool YELLOW = false;
	bool ORANGE = false;
	bool RED = false;
	bool BROWN = false;

	bool LERPING = false;

	float perc = 1.0f;
	float temp = 1.0f;
	float cur = 1.0f;;

	Player* player;

public:
	HealthBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg);

	virtual void Update(float dt);
	virtual void Draw(glm::vec2 scrn);

};

class CrowdBar : public UI {
	static const int HEIGHT;
	static const int MAX_WIDTH;
	const float MAX_TIME = 300.0f; // 5 minutes
	float time = MAX_TIME;

	Player* player;

public:
	CrowdBar(Player* p, glm::vec2 pos, Material* ma, UI* _bg);

	virtual void Update(float dt);
	virtual void Draw(glm::vec2 scrn);
};
