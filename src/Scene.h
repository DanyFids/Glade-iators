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
	static bool loaded;

	//Menu Variables
	int menuSpot[2]{ 0, 0 };
	int MAX_MENU;
	int MIN_MENU;
	const float MENU_TIME = 0.2f;
	float menu_time[2];
	bool _Abutton[2]{ false, false };
	bool _Bbutton[2]{ false, false };
	const int MAX_W = 2;
	const int MIN_W = 0;
	const int MAX_S = 2;
	const int MIN_S = 0;
	int weapon[2]{ 0, 0 };
	int shield[2]{ 0, 0 };
	bool ready[2]{ false, false };
	bool readyChange[2]{ false, false };
	bool changeW[2]{ false, false };
	bool changeS[2]{ false, false };
	bool rightArrow = true;
	bool arrowUsed = false;

	int resolution = 0;
	const int MAX_RES = 4;

	//Cool Settings stuff
	float sensitivity = 1.0f;

	//MAIN MENU
	Material* buttonPlay = new Material("playButton.png");
	Material* buttonSettings = new Material("settingsButton.png");
	Material* buttonExit = new Material("exitButton.png");
	Material* buttonCredits = new Material("creditsButton.png");
	Material* buttonPlay2 = new Material("playButton2.png");
	Material* buttonSettings2 = new Material("settingsButton2.png");
	Material* buttonExit2 = new Material("exitButton2.png");
	Material* buttonCredits2 = new Material("creditsButton2.png");

	Material* buttonPlayRed = new Material("playButtonRed.png");
	Material* buttonSettingsRed = new Material("settingsButtonRed.png");
	Material* buttonExitRed = new Material("exitButtonRed.png");
	Material* buttonCreditsRed = new Material("creditsButtonRed.png");

	//CHARACTER MENU
	Material* buttonReady = new Material("readyButton.png");
	Material* buttonRandom = new Material("randomButton.png");
	Material* buttonReady2 = new Material("readyButton2.png");
	Material* buttonRandom2 = new Material("randomButton2.png");
	Material* backDropMain = new Material("backdrop2.png");

	Material* buttonReadyRed = new Material("readyButtonRed.png");
	Material* buttonReadyBlue = new Material("readyButtonBlue.png");
	Material* buttonReadyPurple = new Material("readyButtonPurple.png");
	Material* buttonRandomRed = new Material("randomButtonRed.png");
	Material* backDropRed = new Material("backdrop2Red.png");
	Material* buttonRandomBlue = new Material("randomButtonBlue.png");
	Material* backDropBlue = new Material("backdrop2Blue.png");

	Material* arrow = new Material("arrow.png");
	Material* arrowBack = new Material("arrow (1).png");
	Material* arrow2 = new Material("arrow2.png");
	Material* arrowBack2 = new Material("arrow (1)2.png");
	Material* ehks = new Material("ehks.png");
	Material* check = new Material("checkmark.png");

	Material* swordIcon = new Material("iconSword.png");
	Material* spearIcon = new Material("iconSpear.png");
	Material* tridentIcon = new Material("iconTrident.png");
	Material* hammerIcon = new Material("iconHammer.png");
	Material* daggerIcon = new Material("iconDagger.png");

	Material* bucklerIcon = new Material("iconBuckler.png");
	Material* shieldIcon = new Material("iconShield.png");
	Material* nothingIcon = new Material("nothing.png");

	TextRenderer* Textcontroller = new TextRenderer();

	/******************/
	/* Menu Variables */
	/******************/

	//Players
	UI* playerOne;
	UI* playerTwo;
	//Menu 1
	UI* play_Button;
	UI* settings_Button;
	UI* credits_Button;
	UI* exit_Button;

	UI* resolution_Button;
	//Menu 2
	UI* random1_Button;
	UI* random2_Button;
	UI* weapon1_Button;
	UI* weapon2_Button;
	UI* shield1_Button;
	UI* shield2_Button;
	UI* ready_Button;
	//Arrows
	UI* arrow_Button1;
	UI* arrow_Button2;
	UI* arrow_Button3;
	UI* arrow_Button4;
	UI* arrow_Button5;
	UI* arrow_Button6;
	UI* arrow_Button7;
	UI* arrow_Button8;
	//Icons
	UI* wOne;
	UI* wTwo;
	UI* sOne;
	UI* sTwo;
	UI* wOne_p1;
	UI* wTwo_p1;
	UI* sOne_p1;
	UI* sTwo_p1;
	UI* wOne_p2;
	UI* wTwo_p2;
	UI* sOne_p2;
	UI* sTwo_p2;
	UI* p1Ready;
	UI* p2Ready;
	
	std::string WeaponName[2];
	std::string ShieldName[2];

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