#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include<vector>

class Scene;
class Mesh;

enum SCENES {
	MAIN_MENU = 0,
	PLAY_SCENE = 1,
	CHARACTER_SCENE = 2,
	CREDITS = 3
};

class Game {
public:
	static glm::ivec2 SCREEN;
	static Game* CURRENT;
	void setScene(SCENES scn);
	static Mesh* QUAD;

	Game();
	~Game();

	void Run();

	GLFWwindow* GetWindow() { return window; }

	friend void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height);
protected:
	void Initialize();
	void Shutdown();

	void LoadContent();
	void UnloadContent();

	void InitImGui();
	void ShutdownImGui();

	void ImGuiNewFrame();
	void ImGuiEndFrame();

	void InputHandle(float dt);
	//void ControllerInput(unsigned int controller, int player, float dt);

	void Update(float deltaTime);
	void Draw();
	void DrawGui(float deltaTime);
private:
	// Stores the main window that the game is running in
	GLFWwindow* window;
	// Stores the clear color of the game's window
	glm::vec4 myClearColor;
	// Stores the title of the game's window
	char windowTitle[32];

	Scene* curScene;

	Scene* OnePlayerScn;
	Scene* TwoPlayerScn;
	Scene* MainMenuScn;
	Scene* CharacterScn;
	Scene* CreditsScn;

	bool wireframe = false;
	bool w_pressed = false;
	//bool f2_pressed = false;
	bool f11_pressed = false;
	bool space_pressed = false;

};