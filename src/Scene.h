#pragma once
#include<vector>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include<entt.hpp>
#include "Object.h"
#include "Hitbox.h"
#include "Texture.h"
#include "Mesh.h"
#include "Text.h"
#include "Sound.h"

class Camera;
class Shader;
class Object;
class PointLight;
class DirectionalLight;
class UI;
class Player;
class Material;
class Hitbox;
class Button;
class ButtonSelect;
class FrameBuffer;
class PostProcess;

class SceneD {
public:
	SceneD() = default;
	virtual ~SceneD() = default;
	virtual void OnOpen() {};
	virtual void OnClose() {};
	entt::registry& Registry() { return myRegistry; }
	const std::string& GetName() const { return myName; }
	void SetName(const std::string& name) { myName = name; }
private:
	entt::registry myRegistry;
	std::string myName;
};

class Scene {
protected:
	std::vector<Camera*> Cam;
	int menuSpot[2]{ 0, 0 };
	int MAX_MENU;
	int MIN_MENU;
	const float MENU_TIME = 0.2f;
	float menu_time[2];
	bool _Abutton[2]{ false, false };
	bool _Bbutton[2]{ false, false };
	const int MAX_W = 4;
	const int MIN_W = 0;
	const int MAX_S = 2;
	const int MIN_S = 0;
	int weapon[2]{ 0, 0 };
	int shield[2]{ 0, 0 };
	bool changeW[2]{ false, false };
	bool changeS[2]{ false, false };
	static bool loaded;

	Material* swordIcon = new Material("iconSword.png");
	Material* spearIcon = new Material("iconSpear.png");
	Material* tridentIcon = new Material("iconTrident.png");
	Material* hammerIcon = new Material("iconHammer.png");
	Material* daggerIcon = new Material("iconDagger.png");

	Material* bucklerIcon = new Material("iconBuckler.png");
	Material* shieldIcon = new Material("iconShield.png");
	Material* nothingIcon = new Material("nothing.png");

	TextRenderer* Textcontroller = new TextRenderer();

	static int P1wins;
	static int P2wins;
	static int RoundCount;

	static Sound* audioEngine;
	static bool AEinit;

	ButtonSelect* playerOne;
	ButtonSelect* playerTwo;

	Button* wOne;
	Button* wTwo;
	Button* sOne;
	Button* sTwo;

	Button* wOne_p1;
	Button* wTwo_p1;
	Button* sOne_p1;
	Button* sTwo_p1;
	Button* wOne_p2;
	Button* wTwo_p2;
	Button* sOne_p2;
	Button* sTwo_p2;
	

	// Mouse Vars
	float m_lastX = 400;
	float m_lastY = 300;
	bool firstMouse = true;
public:

	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) = 0;
	virtual void KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt) = 0;
	virtual void ControllerInput(unsigned int controller, int player, float dt) = 0;
	virtual void Update(float dt) = 0;
	virtual void Draw() = 0;
	virtual void LoadScene() = 0;
	virtual std::vector<Camera*> GetCams() { return Cam; }
	virtual void ResizeCams();
};

class MenuItem {
	bool disabled;
public:
	virtual void Use() = 0;
	virtual void Draw() = 0;
	void setDisabled(bool d) { disabled = d; }
	bool isDisabled() { return disabled; };
};

class MenuScene : public Scene {
protected:
	int selected;
	std::vector<MenuItem*> items;

public:
	// Inherited via Scene
	virtual void KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt) override;
	virtual void ControllerInput(unsigned int controller, int player, float dt) override;
	virtual void Update(float dt) override;
	virtual void Draw() override;
	virtual void LoadScene() override;

	void SelectItem(int item);

	// Inherited via Scene
	virtual void InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt) override;
};

class PlayScene : public Scene {
protected:
	std::vector<Player*> players;
	std::vector<Object*> terrain;
	std::vector<Object*> shields;
	//std::vector<Attack*> attacks;
	std::vector<Object*> weapons;
	std::vector<glm::vec3> beacons;

	bool dodge1 = true;
	bool dodge2 = true;
	float dodge1t;
	float dodge2t;

	bool atk1 = false;
	bool atk2 = false;

	bool block1 = false;
	bool block2 = false;

	bool Target1 = false;
	bool Target2 = false;

	bool isMenu;
	bool ChangingScn = false;


	int CgradeI = 0;
	bool CgradeIDown[3] = { false,false,false };
	int lutloaded = 0;
	

	SplineMan* DUUDE;

	Mesh* Amesh = new Mesh("d6.obj");
	Material* Amat = new Material("missing_tex.png");
	Material* Bmat = new Material("green.png");
	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);

	std::vector<PointLight*> lights;
	DirectionalLight * sun;

	FrameBuffer* main_pass;
	std::vector<PostProcess*> post_pass;

	std::vector<UI*> ui;

public:
	PlayScene();

	static Shader* POST_OUT;

	virtual void KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt) override;
	virtual void ControllerInput(unsigned int controller, int player, float dt) override;
	void RenderScene(Shader* shader, Shader* playerShader);
};