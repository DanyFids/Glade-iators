#pragma once
#include "Scene.h"
#include <thread>
#include "Sound.h"
#include "Object.h"

class Light;
class Mesh;
class ParticleEngine;

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
	FrameBuffer* particle_buff;
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

	Shader* sunShader;

	//Sound audioEngine;

	float deathtimer = 4;
	bool winannounce = false;

	Object* morphyBoi;

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
};
