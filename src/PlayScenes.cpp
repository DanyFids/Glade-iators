#include "PlayScenes.h"
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <iostream>
#include <typeinfo>


#include"Mesh.h"
#include"Shader.h"
#include"Texture.h"
#include"Camera.h"
#include"Light.h"
#include"Object.h"
#include"Mesh.h"
#include"Constants.h"
#include"Test_Primitives.h"
#include"Hitbox.h"
#include"UI.h"


OnePlayer::OnePlayer()
{
	LoadScene();
}

void OnePlayer::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
		ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
	}
	else {
		KeyboardInput(window, mousePos, PLAYER_1, dt);
	}

	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS && !f3_pressed) {
		if (debug) {
			if (disp_depth != lights.size() - 1)
				disp_depth++;
			else
				debug = false;
		}
		else {
			if (lights.size() > 0) {
				debug = true;
				disp_depth = 0;
			}
		}

		f3_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
		f3_pressed = false;
}

void OnePlayer::Update(float dt)
{
	for (int c = 0; c < players.size(); c++) {
		players[c]->Update(dt);

		for (int p = 0; p < players.size(); p++) {
			if (players[c] != players[p]) {
				if (players[c]->HitDetect(players[p])) {
					//std::cout << "Welp\n";
				}
			}
		}

		if (glfwJoystickPresent(c) && glfwJoystickIsGamepad(c)) {
			Cam[c]->Move(players[c]->phys.move, dt);


			players[c]->ApplyMove();

			Cam[c]->SetTarget(players[c]->GetPosition());
		}
	}

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	((MorphMesh*)(morphyBoi->GetMesh()))->Update(dt);
	// m morphyBoi->Update(dt);
}

void OnePlayer::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	RenderScene(sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		RenderScene(depthShader);
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
	morphShader->SetI("num_lights", lights.size());

	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		RenderScene(shaderObj);

		Cam[c]->SetupCam(morphShader);
		morphyBoi->Draw(morphShader, Cam);
		staticBoi->Draw(shaderObj, Cam);
		//staticBoi->Draw(shaderObj, Cam);
	}

	glDisable(GL_DEPTH_TEST);
	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void OnePlayer::LoadScene()
{
	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");

	Material* stamBarMat = new Material("green.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("TreePersonThing.obj");
	

	Hitbox* basicCubeHB = new CubeHitbox(1.0f,1.0f,1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);
	players.push_back(new Player(Square, DiceTex, basicCubeHB, { 3.0f, 0.3f, 0.0f }));
	players.push_back(new Object(d20, D20Tex, basicSphereHB));

	players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, 0.3f, 0.0f });
	

	players.push_back(new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	players[2]->Scale(glm::vec3(0.1f));

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	floor->Move({ 0.0f, -0.75f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	terrain.push_back(floor);

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, SCREEN_WIDTH, SCREEN_HEIGHT)) 
	};

	ui = {
		new StaminaBar((Player*)players[PLAYER_1], glm::vec2(50, 500), stamBarMat)
	};
 

	std::vector<std::string> frames = { "wobble/wobble1.obj", "wobble/wobble2.obj", "wobble/wobble3.obj", "wobble/wobble4.obj" };

	morphyBoi = new Object(new MorphMesh(frames), defaultTex, basicCubeHB, glm::vec3(2.0f,1.0f,2.0f));
	staticBoi = new Object(new Mesh("wobble/wobble2.obj"), defaultTex, basicCubeHB, glm::vec3(2.0f, 4.0f, 2.0f));

	// DEBUG THINGS
	DebugShader = new Shader("Shaders/debug.vert", "Shaders/debug.frag");
	DebugQuad = new Mesh(square, 4, square_index, 6);
}




/*******************************************************************************************
*	Two Player Scene Functions
*******************************************************************************************/

TwoPlayer::TwoPlayer()
{
	LoadScene();
}

void TwoPlayer::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) && glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
		ControllerInput(GLFW_JOYSTICK_1, PLAYER_1, dt);
	}
	else {
		KeyboardInput(window, mousePos, PLAYER_2, dt);
	}

	if(glfwJoystickPresent(GLFW_JOYSTICK_2) && glfwJoystickIsGamepad(GLFW_JOYSTICK_2)) {
		ControllerInput(GLFW_JOYSTICK_2, PLAYER_2, dt);
	}
}

void TwoPlayer::Update(float dt)
{
	if (players[PLAYER_1]->hitbox->HitDetect(players[PLAYER_1]->GetTransform(), (CubeHitbox*)(players[PLAYER_2]->hitbox), players[PLAYER_2]->GetTransform())) {
		std::cout << "HOII! HIT DETECTED!" << std::endl;
	}

	//Square->Update(dt);
}

void TwoPlayer::Draw()
{
	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
	}
	shaderObj->SetI("num_lights", lights.size());

	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		RenderScene(shaderObj);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void TwoPlayer::LoadScene()
{
	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");

	Material* DiceTex = new Material("d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");

	//sun = new DirectionalLight(glm::normalize(glm::vec3(1.5f, 1.0f, 0.5f)), { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f);
	//lights.push_back(new PointLight({ 0.0f, 30.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 1.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);

	players.push_back(new Object(Square, DiceTex, basicCubeHB));
	players.push_back(new Object(d20, D20Tex, basicCubeHB));

	players[PLAYER_2]->Scale(glm::vec3(0.75f, 0.75f, 0.75f));

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	floor->Move({ 0.0f, -1.5f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	terrain.push_back(floor);

	Cam = {
		new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec4(0,0, SCREEN_WIDTH / 2, SCREEN_HEIGHT)), // Cam 1
		new Camera(glm::vec3(2.0f, 0.0f, -4.0f), glm::vec4(SCREEN_WIDTH / 2,0, SCREEN_WIDTH / 2, SCREEN_HEIGHT)) // Cam 2
	};
}
