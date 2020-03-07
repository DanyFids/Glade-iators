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
class ButtonSelect;

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
	bool _Abutton[2] {false, false};
	bool _Bbutton[2] {false, false};
	TextRenderer* Textcontroller = new TextRenderer();

	ButtonSelect* playerOne;
	ButtonSelect* playerTwo;

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

	SplineMan* DUUDE;

	Mesh* Amesh = new Mesh("d6.obj");
	Material* Amat = new Material("missing_tex.png");
	Material* Bmat = new Material("green.png");
	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);

	std::vector<PointLight*> lights;
	DirectionalLight * sun;


	std::vector<UI*> ui;

public:
	virtual void KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt) override;
	virtual void ControllerInput(unsigned int controller, int player, float dt) override;
	void RenderScene(Shader* shader);
};