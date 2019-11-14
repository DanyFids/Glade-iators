#pragma once
#include<string>
#include<vector>
#include<GLM/glm.hpp>
class Mesh;
class Material;
class Shader;
class Camera;
class Player;

class UI {
protected:
	static Mesh* QUAD;
	static glm::mat4 projection;
	glm::vec2 pos;
	glm::vec2 dim;

	glm::mat4 model;
	Material* material;

public:
	static Shader* SHADER;

	static void INIT();

	UI(int width, int height, glm::vec2 pos, Material* ma);
	virtual void Draw(glm::vec2 scrn);
	virtual void Update(float dt) {};
};

class StaminaBar : public UI {
	static const int HEIGHT;
	static const int MAX_WIDTH;

	Player* player;

public:
	StaminaBar(Player* p, glm::vec2 pos, Material* ma);

	virtual void Update(float dt);
};