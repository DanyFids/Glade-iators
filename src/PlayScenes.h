#pragma once
#include "Scene.h"
#include <thread>
#include "Sound.h"
#include "Object.h"

class Light;
class Mesh;
class ParticleEngine;
class CrowdBar;

class OnePlayer: public PlayScene {
	Shader* shaderObj;
	Shader* depthShader;
	Shader* sunShader;
	Shader* skelShader;
	Shader* skelDepth;
	Shader* morphShader;
	Shader* lightPass;

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

	ParticleEngine* fire;

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
	Shader* skelDepth;
	Shader* lightPass;
	Shader* sunShader;

	const float MAX_SCORE = 500.0f;
	float curScore = MAX_SCORE;
	float scoreSub = 1.0f;
	const float INCREASE_TIME = 7.5f;
	float subIncreaser = INCREASE_TIME;
	
	const float MAX_TAUNT = 15.0f;
	float tauntTime[2]{ 0.0f, 0.0f };
	float taunted[2]{ 1.0f, 1.0f };

	float MAX_COMBO = 5.0f;
	float comboTime[2]{ MAX_COMBO, MAX_COMBO };
	float comboMult[2]{ 1.0f, 1.0f };
	bool combo[2]{ false, false };
	//Sound audioEngine;
	const float DEATH_TIME = 4.0f;
	float deathtimer = DEATH_TIME;
	bool winannounce = false;

	

	Object* morphyBoi;
	CrowdBar* CrowdBoi = nullptr;

	Sound audioEngine;

	WeaponType _Weapons[2];
	ShieldType _Shields[2];

public:
	TwoPlayer(WeaponType p1_weapon = WEAPON_SWORD, ShieldType p1_shield = SHIELD_LARGE, WeaponType p2_weapon = WEAPON_SWORD, ShieldType p2_shield = SHIELD_LARGE);
	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;

	virtual void ResizeCams();

	virtual void Reset();
};
