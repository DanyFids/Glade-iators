#pragma once
#include "Scene.h"

class Light;
class Mesh;
class Player;

class OnePlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* sunShader;
	Shader* skelShader;

	Shader* DebugShader;
	Mesh* DebugQuad;
	
	Player* test_player;

	bool debug = false;
	bool f3_pressed;
	int disp_depth = 0;

public:
	OnePlayer();

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};

class TwoPlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;

public:
	TwoPlayer();
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};