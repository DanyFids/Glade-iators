#pragma once
#include "Scene.h"
#include <thread>

class Light;
class Mesh;
class Player;

class OnePlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* sunShader;
	Shader* skelShader;
	Shader* skelDepth;

	Shader* DebugShader;
	Mesh* DebugQuad;
	
	Player* test_player;

	bool debug = false;
	bool f3_pressed;
	int disp_depth = 0;
	
	std::thread threadObj;

	//CG ASSIGNMENT STUFF
	bool cg_asg_input = false;
	bool enable_ambient = true;
	bool enable_diffuse = true;
	bool enable_spec = true;
	bool enable_rim = true;
public:
	OnePlayer();

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;

	Player* GetPlayer(int i) { return (Player*) players[i]; }
	Player* GetTestPlayer() { return test_player; }

	// CG ASSINGMENT FUNCS

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