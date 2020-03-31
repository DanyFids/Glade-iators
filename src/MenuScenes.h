#pragma once

#include "Scene.h"

class Light;
class Mesh;
class Player;


class MainMenu : public MenuScene {

protected:
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

class Credits : public MenuScene {

protected:
	Shader* shaderObj;
	Shader* depthShader;
	Shader* morphShader;
	Shader* sunShader;
	Shader* skelDepth;

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

class CharacterC : public MenuScene {

protected:
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

class SettingsScene : public MenuScene {

protected:
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