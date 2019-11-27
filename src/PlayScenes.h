#pragma once
#include "Scene.h"

class Light;
class Mesh;

class OnePlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* sunShader;

	Shader* DebugShader;
	Mesh* DebugQuad;
	bool debug = false;
	bool f3_pressed;
	int disp_depth = 0;

	float time = 0.0f;
	float MAX_TIME = 0.2f;

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

	Shader* sunShader;

public:
	TwoPlayer();
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};