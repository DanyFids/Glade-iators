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
	Shader* morphShader;
	Shader* lightPass;

	FrameBuffer* light_buff;
	FrameBuffer* merge_buff;
	int prev_num_l = 0;
	const int MAX_LIGHTS = 25;
	int active_lights = 1;

	Shader* DebugShader;
	Shader* vergausshader;
	Shader* horgausshader;
	Shader* highlightshader;
	Shader* bloomshader;
	Shader* pixelshader;

	Shader* LUT_SHADER;
	LUT* lut_cool;
	LUT* lut_hot;
	LUT* lut_custom;

	Mesh* DebugQuad;

	float deathtimer = 4;
	
	Player* test_player;

	Player* test_bones;

	enum CG_TOGGLE {
		CG_OUTPUT, CG_COLOR, CG_NORMALS, CG_DEPTH, CG_LIGHTS
	} toggle = CG_OUTPUT;

	bool show_volumes = true;
	bool move_lights = false;
	bool debug = false;
	bool f3_pressed;
	int disp_depth = 0;
	
	std::thread threadObj;

	Object* morphyBoi;
	Object* staticBoi;



	float time = 0.0f;
	float MAX_TIME = 0.2f;

public:
	OnePlayer();

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;

	void Load_Lights_From_File(std::string f);

	virtual void ResizeCams();

	Player* GetPlayer(int i) { return (Player*) players[i]; }
	Player* GetTestPlayer() { return test_player; }

	// CG ASSINGMENT FUNCS

};

class TwoPlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* skelShader;

	Shader* sunShader;


	//Sound audioEngine;

	float deathtimer = 4;
	bool winannounce = false;

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
	UI* logos;
	UI* blackBox;

	const float MAX_TIME = 2.0f;
	float menu_time = 1.0f;

public:
	
	MainMenu();
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};

class Credits : public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* sunShader;
	Shader* skelDepth;
	UI* spaget;

	const float MAX_TIME = 3.0f;
	float menu_time = MAX_TIME;

	bool displayed = false;
public:
	Credits();

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

	std::string Name1[3];
	std::string Name2[3];

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};

class SettingsScene : public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* sunShader;
	Shader* skelDepth;

public:
	SettingsScene();

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;
};