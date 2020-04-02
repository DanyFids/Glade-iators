#include "MenuScenes.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include <typeinfo>
#include <random>
#include <thread>
#include <queue>
#include <florp/app/timing.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Constants.h" 
#include "UI.h"
#include "Lerp.h"
#include "Light.h" 
#include "Sound.h"
#include "Game.h"
#include "PostProcess.h"
#include "Text.h" 


MainMenu::MainMenu()
{
	LoadScene();
}

void MainMenu::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
		}
		else {
			KeyboardInput(window, mousePos, PLAYER_2, dt);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
			ControllerInput(GLFW_JOYSTICK_2, PLAYER_2, dt);
		}
	}
	else {

		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);

		}
		else {
			KeyboardInput(window, mousePos, PLAYER_1, dt);
		}

	}
}

void MainMenu::Update(float dt)
{

	static bool displayed = false;
	static bool displayLogo = false;
	//audioEngine.Update();

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

	if (logos != nullptr) {
		if (menu_time <= 0) {
			if (!displayLogo) {
				displayLogo = true;
				menu_time = MAX_TIME;
			}
			else if (!displayed) {
				displayed = true;
				menu_time = MAX_TIME;
			}
		}
		else {
			menu_time -= dt;
		}
		if (displayLogo)
			blackBox->setOpacity(menu_time / MAX_TIME);
		if (displayed) {
			blackBox->setOpacity(0);
			logos->setOpacity(menu_time / MAX_TIME);
			ChangingScn = false;
		}
	}

}

void MainMenu::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	//sun->SetupDepthShader(skelDepth);

	//RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		//RenderScene(depthShader, sunShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0,  , SCREEN_HEIGHT);

		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(morphShader);

	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);

	}
	shaderObj->SetI("num_lights", lights.size());
	//morphShader->SetI("num_lights", lights.size());


	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		//RenderScene(shaderObj, sunShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);
	}


	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	if (logos != nullptr) {
		logos->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
		blackBox->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "", 25.0f, 25.0f, 1.0f, glm::vec3(1.f, 1.f, 1.f));

	glEnable(GL_DEPTH_TEST);

}

void MainMenu::LoadScene()
{
	ChangingScn = true;
	MAX_MENU = 0;
	MIN_MENU = -3;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* blackBarMat = new Material("black.png");

	Material* gladeiatorsTitle = new Material("Title.png");

	Material* firstPlayer = new Material("redPlayer.png");
	Material* secondPlayer = new Material("bluePlayer.png");
	Material* buttonPlay = new Material("playButton.png");
	Material* buttonSettings = new Material("settingsButton.png");
	Material* buttonExit = new Material("exitButton.png");
	Material* buttonCredits = new Material("creditsButton.png");
	Material* titleImage = new Material("gladewallpaper.png");
	Material* menuImage = new Material("blackMenu.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y))
	};

	//playerOne = new UI(210, 80, glm::vec3(0, 435, 0), firstPlayer);
	//playerTwo = new UI(210, 80, glm::vec3(0, 275, 0), firstPlayer);
	play_Button = new UI(200, 70, glm::vec3(5, 440, 0), buttonPlay);
	settings_Button = new UI(200, 70, glm::vec3(5, 320, 0), buttonSettings);
	credits_Button = new UI(200, 70, glm::vec3(5, 200, 0), buttonCredits);
	exit_Button = new UI(200, 70, glm::vec3(5, 80, 0), buttonExit);

	ui = {
		new UI(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f), titleImage),
		//playerOne,
		//playerTwo,
		play_Button,
		settings_Button,
		credits_Button,
		exit_Button
	};

	if (!loaded) {
		logos = new UI(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f), gladeiatorsTitle);
		blackBox = new UI(750, 400, glm::vec3(0, 0, 0), menuImage);
	}
}

CharacterC::CharacterC()
{
	LoadScene();
}

void CharacterC::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
		}
		else {
			KeyboardInput(window, mousePos, PLAYER_2, dt);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
			ControllerInput(GLFW_JOYSTICK_2, PLAYER_2, dt);
		}
	}
	else {

		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);

		}
		else {
			KeyboardInput(window, mousePos, PLAYER_1, dt);
		}

	}
}

void CharacterC::Update(float dt)
{

	//audioEngine.Update();

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

}

void CharacterC::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	//sun->SetupDepthShader(skelDepth);

	//RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		//RenderScene(depthShader, sunShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(morphShader);

	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);

	}
	shaderObj->SetI("num_lights", lights.size());
	//morphShader->SetI("num_lights", lights.size());


	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		//RenderScene(shaderObj, sunShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);

	}

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name1[0], 385.0f - (Name1[0].length() * 4), 570.0f, 0.45f, glm::vec3(1.f, 0.0f, 0.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name1[1], 385.0f - (Name1[1].length() * 4), 540.0f, 0.45f, glm::vec3(1.f, 0.0f, 0.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name1[2], 385.0f - (Name1[2].length() * 4), 510.0f, 0.45f, glm::vec3(1.f, 0.0f, 0.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "VS", 370.0f, 470.0f, 0.75f, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name2[0], 385.0f - (Name2[0].length() * 4), 430.0f, 0.45f, glm::vec3(0.5f, 0.5f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name2[1], 385.0f - (Name2[1].length() * 4), 400.0f, 0.45f, glm::vec3(0.5f, 0.5f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name2[2], 385.0f - (Name2[2].length() * 4), 370.0f, 0.45f, glm::vec3(0.5f, 0.5f, 1.f));

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, WeaponName[0], 174.0f - (WeaponName[0].length() * 4), 227.5f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, WeaponName[1], 615.0f - (WeaponName[1].length() * 4), 227.5f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, ShieldName[0], 176.0f - (ShieldName[0].length() * 4), 142.5f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, ShieldName[1], 617.0f - (ShieldName[1].length() * 4), 142.5f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
	glEnable(GL_DEPTH_TEST);

}

void CharacterC::LoadScene()
{
	ChangingScn = false;

	MAX_MENU = 10;
	menuSpot[0] = 10;
	menuSpot[1] = 10;
	MIN_MENU = 7;

	srand(time(NULL));

	Name1[0] = Textcontroller->GenerateTitle();
	Name1[1] = Textcontroller->GenerateName();
	Name1[2] = Textcontroller->GenerateSuffix();

	Name2[0] = Textcontroller->GenerateTitle();
	Name2[1] = Textcontroller->GenerateName();
	Name2[2] = Textcontroller->GenerateSuffix();

	WeaponName[0] = "Sword";
	WeaponName[1] = "Sword";
	ShieldName[0] = "Shield";
	ShieldName[1] = "Shield";

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* firstPlayer = new Material("redPlayer.png");
	Material* secondPlayer = new Material("bluePlayer.png");
	Material* buttonReady = new Material("readyButton.png");
	Material* buttonRandom = new Material("randomButton.png");

	Material* background = new Material("backgroundWood.png");
	Material* backDrop = new Material("backdrop.png");
	Material* backDropMain = new Material("backdrop2.png");
	Material* backDropFade = new Material("backdrop3.png");
	Material* nothing = new Material("nothing.png");

	Material* swordIcon = new Material("iconSword.png");
	Material* hammerIcon = new Material("iconHammer.png");
	Material* spearIcon = new Material("iconSpear.png");
	Material* shieldIcon = new Material("iconShield.png");
	Material* bucklerIcon = new Material("iconBuckler.png");

	Material* tree1 = new Material("treeportrait2.png");
	Material* tree2 = new Material("treeportrait1.png");
	Material* borderWall = new Material("borderPart1.png");

	Material* blackBarMat = new Material("black.png");
	Material* arrow = new Material("arrow.png");
	Material* arrow2 = new Material("arrow (1).png");

	Material* ehks = new Material("Ehks.png");
	//PLAYERS
	//playerOne = new UI(70, 70, glm::vec3(145, 230, 0), firstPlayer);
	//playerTwo = new UI(70, 70, glm::vec3(585, 230, 0), secondPlayer);

	//BUTTONS
	weapon1_Button = new UI(70, 70, glm::vec3(145, 155, 0), backDropMain);
	shield1_Button = new UI(70, 70, glm::vec3(145, 70, 0), backDropMain);
	weapon2_Button = new UI(70, 70, glm::vec3(585, 155, 0), backDropMain);
	shield2_Button = new UI(70, 70, glm::vec3(585, 70, 0), backDropMain);

	ready_Button = new UI(580, 60, glm::vec3(110, 10, 0), buttonReady);
	random1_Button = new UI(140, 60, glm::vec3(100, 235, 0), buttonRandom);
	random2_Button = new UI(140, 60, glm::vec3(540, 235, 0), buttonRandom);

	//ARROWS
	arrow_Button1 = new UI(40, 50, glm::vec3(35, 168, 0), arrow);
	arrow_Button2 = new UI(40, 50, glm::vec3(35, 88, 0), arrow);
	arrow_Button3 = new UI(40, 50, glm::vec3(285, 168, 0), arrow2);
	arrow_Button4 = new UI(40, 50, glm::vec3(285, 88, 0), arrow2); // PLAYER 1 ARROWS
	arrow_Button5 = new UI(40, 50, glm::vec3(475, 168, 0), arrow);
	arrow_Button6 = new UI(40, 50, glm::vec3(475, 88, 0), arrow);
	arrow_Button7 = new UI(40, 50, glm::vec3(725, 168, 0), arrow2);
	arrow_Button8 = new UI(40, 50, glm::vec3(725, 88, 0), arrow2); // PLAYER 2 ARROWS

	//ICONS
	wOne = new UI(60, 60, glm::vec3(150, 160, 0), swordIcon);
	sOne = new UI(50, 50, glm::vec3(155, 80, 0), shieldIcon);
	wTwo = new UI(60, 60, glm::vec3(590, 160, 0), swordIcon);
	sTwo = new UI(50, 50, glm::vec3(595, 80, 0), shieldIcon);
	p1Ready = new UI(40, 40, glm::vec3(60, 20, 0), ehks);
	p2Ready = new UI(40, 40, glm::vec3(695, 20, 0), ehks);
	//Side Icons
	wOne_p1 = new UI(50, 50, glm::vec3(90, 165, 0), hammerIcon);
	sOne_p1 = new UI(40, 40, glm::vec3(95, 85, 0), nothing);
	wTwo_p1 = new UI(50, 50, glm::vec3(530, 165, 0), hammerIcon);
	sTwo_p1 = new UI(40, 40, glm::vec3(535, 85, 0), nothing);
	wOne_p2 = new UI(50, 50, glm::vec3(220, 165, 0), spearIcon);
	sOne_p2 = new UI(40, 40, glm::vec3(225, 85, 0), bucklerIcon);
	wTwo_p2 = new UI(50, 50, glm::vec3(660, 165, 0), spearIcon);
	sTwo_p2 = new UI(40, 40, glm::vec3(665, 85, 0), bucklerIcon);


	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y))
	};

	ui = {
		new UI(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f), background),
		//playerOne,
		//playerTwo,

		weapon1_Button,
		shield1_Button,
		weapon2_Button,
		shield2_Button,

		ready_Button,
		random1_Button,
		random2_Button,

		new UI(50, 50, glm::vec3(90, 165, 0), backDrop),
		new UI(50, 50, glm::vec3(90, 80, 0), backDrop),
		new UI(50, 50, glm::vec3(530, 165, 0), backDrop),
		new UI(50, 50, glm::vec3(530, 80, 0), backDrop), //4
		new UI(50, 50, glm::vec3(220, 165, 0), backDrop),
		new UI(50, 50, glm::vec3(220, 80, 0), backDrop),
		new UI(50, 50, glm::vec3(660, 165, 0), backDrop),
		new UI(50, 50, glm::vec3(660, 80, 0), backDrop), //4

		wOne_p1,
		sOne_p1,
		wTwo_p1,
		sTwo_p1, //4
		wOne_p2,
		sOne_p2,
		wTwo_p2,
		sTwo_p2, //4

		wOne,
		sOne,
		wTwo,
		sTwo,

		new UI(800, 400, glm::vec3(0, 305, 0), blackBarMat),
		new UI(180, 280, glm::vec3(80, 310, 0), tree1),
		new UI(180, 280, glm::vec3(520, 310, 0), tree2),
		new UI(260, 328, glm::vec3(40, 305, 0), borderWall),
		new UI(260, 328, glm::vec3(480, 305, 0), borderWall),

		arrow_Button1,
		arrow_Button2,
		arrow_Button3,
		arrow_Button4,
		arrow_Button5,
		arrow_Button6,
		arrow_Button7,
		arrow_Button8,

		new UI(50, 50, glm::vec3(90, 165, 0), backDropFade),
		new UI(50, 50, glm::vec3(90, 80, 0), backDropFade),
		new UI(50, 50, glm::vec3(530, 165, 0), backDropFade),
		new UI(50, 50, glm::vec3(530, 80, 0), backDropFade), //4
		new UI(50, 50, glm::vec3(220, 165, 0), backDropFade),
		new UI(50, 50, glm::vec3(220, 80, 0), backDropFade),
		new UI(50, 50, glm::vec3(660, 165, 0), backDropFade),
		new UI(50, 50, glm::vec3(660, 80, 0), backDropFade), //4

		p1Ready,
		p2Ready
	};

	////Final Chunck
	ui[41]->setOpacity(0.5);
	ui[42]->setOpacity(0.5);
	ui[43]->setOpacity(0.5);
	ui[44]->setOpacity(0.5);
	ui[45]->setOpacity(0.5);
	ui[46]->setOpacity(0.5);
	ui[47]->setOpacity(0.5);
	ui[48]->setOpacity(0.5);


}


SettingsScene::SettingsScene()
{
	LoadScene();
}

void SettingsScene::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
		}
		else {
			KeyboardInput(window, mousePos, PLAYER_2, dt);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
			ControllerInput(GLFW_JOYSTICK_2, PLAYER_2, dt);
		}
	}
	else {

		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);

		}
		else {
			KeyboardInput(window, mousePos, PLAYER_1, dt);
		}

	}
}

void SettingsScene::Update(float dt)
{

	//audioEngine.Update();

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

}

void SettingsScene::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	//sun->SetupDepthShader(skelDepth);

	//RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		//RenderScene(depthShader, sunShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//glViewport(0, 0,  , SCREEN_HEIGHT);

		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(morphShader);

	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);

	}
	shaderObj->SetI("num_lights", lights.size());
	//morphShader->SetI("num_lights", lights.size());


	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		//RenderScene(shaderObj, sunShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);
	}


	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "", 25.0f, 25.0f, 1.0f, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, ResolutionDisplay, 440.0f - (ResolutionDisplay.length() * 4), 550.0f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));

	glEnable(GL_DEPTH_TEST);

}

void SettingsScene::LoadScene()
{
	ChangingScn = false;

	MAX_MENU = 20;
	MIN_MENU = 19;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* blackBarMat = new Material("black.png");
	Material* white = new Material("white.png");
	Material* background = new Material("backgroundWood.png");
	Material* art = new Material("colitreeums.png");
	Material* shrup = new Material("shrup.png");

	Material* gladeiatorsTitle = new Material("Title.png");

	ResolutionDisplay = "1920x1080";

	buttonReturn = new Material("returnButton.png");
	buttonRes = new Material("resolutionButton.png");
	Material* titleImage = new Material("gladewallpaper.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y))
	};

	return_Button = new UI(200, 70, glm::vec3(55, 430, 0), buttonReturn);
	resolution_Button = new UI(200, 70, glm::vec3(55, 520, 0), buttonRes); 
	arrow_Button1 = new UI(40, 50, glm::vec3(5, 530, 0), arrow);
	arrow_Button3 = new UI(40, 50, glm::vec3(260, 530, 0), arrowBack);
	 
	ui = {
		new UI(SCREEN_WIDTH, 600, glm::vec3(0.0f), art),
		new UI(SCREEN_WIDTH, 30, glm::vec3(0, 400,0.0f), blackBarMat),
		//new UI(SCREEN_WIDTH, 420, glm::vec3(0.0f), blackBarMat),
		new UI(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0, 420, 0.0f), background),
		new UI(100, 35, glm::vec3(397.5, 539.5, 0.0f), white),
		new UI(95, 30, glm::vec3(400, 542, 0.0f), blackBarMat),
		new UI(180, 170, glm::vec3(580, 430, 0), shrup),
		return_Button,
		resolution_Button, 
		arrow_Button1,
		arrow_Button3
	};

}

Credits::Credits()
{
	LoadScene();
}

void Credits::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
		}
		else {
			KeyboardInput(window, mousePos, PLAYER_2, dt);
		}

		if (glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
			ControllerInput(GLFW_JOYSTICK_2, PLAYER_2, dt);
		}
	}
	else {

		if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
			ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);

		}
		else {
			KeyboardInput(window, mousePos, PLAYER_1, dt);
		}

	}
}

void Credits::Update(float dt)
{
}

void Credits::Draw()
{

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	//Programmers:\nDylan Brush\nCraig Holder\nAnthony Ona\nArtists:\nMaija Kinnunen\nTia Lee
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "ProjectManager:", 20, 550, 0.75, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Daniel Findleton", 20, 520, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Programmers:", 20, 450, 0.75, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Dylan Brush", 20, 420, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Craig Holder", 20, 390, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Anthony Ona", 20, 360, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Artists:", 20, 290, 0.75, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Maija Kinnunen", 20, 260, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Tia Lee", 20, 230, 0.5, glm::vec3(1.f, 1.f, 1.f));

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Special Thanks", 20, 160, 0.75, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Playtesters:", 20, 125, 0.75, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Zack Hayes", 20, 90, 0.5, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Marian Grippa Almeida", 20, 60, 0.5, glm::vec3(1.f, 1.f, 1.f));
}

void Credits::LoadScene()
{
	ChangingScn = false;

	MAX_MENU = 17;
	MIN_MENU = 17;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* blackBarMat = new Material("black.png");

	Material* buttonReturn = new Material("returnButton.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y))
	};

	return_Button = new UI(200, 70, glm::vec3(550, 50, 0), buttonReturn);

	ui = {
		return_Button
	};

}
