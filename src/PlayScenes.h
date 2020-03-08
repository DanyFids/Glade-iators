#pragma once
#include "Scene.h"
#include <thread>

#include "Sound.h"

class Light;
class Mesh;
class Player;

class OnePlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* sunShader;
	Shader* skelShader;
	Shader* skelDepth;
	Shader* morphShader;
	
	Shader* DebugShader;
	Shader* vergausshader;
	Shader* horgausshader;
	Shader* highlightshader;
	Shader* bloomshader;
	Shader* pixelshader;

	Mesh* DebugQuad;

	float deathtimer = 4;
	
	Player* test_player;

	Player* test_bones;

	bool debug = false;
	bool f3_pressed;
	int disp_depth = 0;
	
	std::thread threadObj;

	Object* morphyBoi;
	Object* staticBoi;
	Sound audioEngine;

	float time = 0.0f;
	float MAX_TIME = 0.2f;

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
	Shader* morphShader;

	Shader* sunShader;


	Object* morphyBoi;
public:
	TwoPlayer();
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};

class MainMenu : public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* sunShader;
	Shader* skelDepth;
public:
	MainMenu();


	std::string Names[2];
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};

class CharacterC : public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* sunShader;
	Shader* skelDepth;

public:
	CharacterC();

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};