#include "PlayScenes.h"
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <thread>
#include <queue>

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
#include"Skeleton.h"

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
		if (!f3_pressed) {
			((SkelMesh*)test_player->GetMesh())->NextFrame(0);
		}

		f3_pressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
		f3_pressed = false;

	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !cg_asg_input) {
		enable_ambient = false;
		enable_diffuse = false;
		enable_spec = false;
		enable_rim = false;

		cg_asg_input = true;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
		cg_asg_input = false;

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !cg_asg_input) {
		enable_ambient = true;
		enable_diffuse = false;
		enable_spec = false;
		enable_rim = false;

		cg_asg_input = true;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
		cg_asg_input = false;

	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !cg_asg_input) {
		enable_ambient = false;
		enable_diffuse = false;
		enable_spec = true;
		enable_rim = false;

		cg_asg_input = true;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
		cg_asg_input = false;

	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !cg_asg_input) {
		enable_ambient = false;
		enable_diffuse = false;
		enable_spec = true;
		enable_rim = true;

		cg_asg_input = true;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
		cg_asg_input = false;

	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !cg_asg_input) {
		enable_ambient = true;
		enable_diffuse = false;
		enable_spec = true;
		enable_rim = true;

		cg_asg_input = true;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE)
		cg_asg_input = false;
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

	test_player->Update(dt);
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
		lights[l]->SetupDepthShader(skelDepth);
		RenderScene(depthShader);
		test_player->Draw(skelDepth, Cam);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(skelShader);
	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(skelShader, c);
	}
	shaderObj->SetI("num_lights", lights.size());
	skelShader->SetI("num_lights", lights.size());

	shaderObj->SetB("enable_a", enable_ambient);
	shaderObj->SetB("enable_d", enable_diffuse);
	shaderObj->SetB("enable_s", enable_spec);
	shaderObj->SetB("enable_r", enable_rim);

	skelShader->SetB("enable_a", enable_ambient);
	skelShader->SetB("enable_d", enable_diffuse);
	skelShader->SetB("enable_s", enable_spec);
	skelShader->SetB("enable_r", enable_rim);

	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);

		RenderScene(shaderObj);

		Cam[c]->SetupCam(skelShader);

		test_player->Draw(skelShader, Cam);

		glDisable(GL_DEPTH_TEST);
		((SkelMesh*)(test_player->GetMesh()))->DrawSkeleton( test_player->GetTransform().GetWorldTransform(), shaderObj);
		glEnable(GL_DEPTH_TEST);
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
	Joint::init();

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Basic_Shader.frag");
	skelDepth = new Shader("Shaders/depth_skel.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-normal.png");
	
	Material* GladiatorWM = new Material("WeightMap.png");

	Material* stamBarMat = new Material("green.png");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "bone_t.bvh");
	SkelMesh* GladiatorMesh = new SkelMesh("gladiator.obj", gladiatorSkel, "WeightMap.png");

	Skeleton* wiggleSkel = new Skeleton("Wiggle_Rig", "wiggle/wigglyboi.bvh");
	SkelMesh* wigglyboi = new SkelMesh("wiggle/wigglyboi.obj", wiggleSkel, "wiggle/WiggleWeights.png");

	gladiatorSkel->WriteTree();
	//gladiatorSkel->Find("l_arm2")->animations[0][0].rotation += glm::vec3(0.0f, -90.0f, 0.0f);
	//gladiatorSkel->Find("l_arm1")->animations[0][0].scale = glm::vec3(2.0f, 2.0f, 2.0f);

	//wiggleSkel->Find("bone2")->animations[0][0].scale = glm::vec3(2.0f, 1.0f, 2.0f);

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.7f, 0.014f, 0.0007f));//lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");

	Hitbox* basicCubeHB = new CubeHitbox(1.0f,1.0f,1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);
	players.push_back(new Object(Square, DiceTex, basicCubeHB, { 3.0f, 0.3f, 0.0f }));
	players.push_back(new Object(d20, D20Tex, basicSphereHB, {0.0f, 0.0f, 0.0f}));

	players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, 0.3f, 0.0f });

	GladiatorMesh->SetAnim(1, 0);
	GladiatorMesh->SetFrame(0, 0);

	//gladiatorSkel->Find("l_arm1")->WriteTransform(1, 1);
	//gladiatorSkel->Find("r_arm1")->WriteTransform(1, 1);

	test_player = new Player(GladiatorMesh, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f });
	test_player->Scale(glm::vec3(1.2f));

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

	// DEBUG THINGS
	DebugShader = new Shader("Shaders/debug.vert", "Shaders/debug.frag");
	DebugQuad = new Mesh(square, 4, square_index, 6);

	// Console?
	auto console = [this]() {
		std::string line = "";
		std::string read;

		std::queue<std::string> input;

		while (true) {
			while (input.size() > 0) {
				input.pop();
			}

			std::cout << "> ";
			std::getline(std::cin, line);

			auto pos = line.find(" ");

			while (pos != std::string::npos) {
				read = line.substr(0, pos);
				line.erase(0, pos+1);

				input.push(read);

				pos = line.find(" ");
			}

			input.push(line);

			if (input.size() > 0) {
				std::string command = input.front();
				input.pop();

				if (command.compare("play") == 0) {
					if (input.size() >= 2) {
						std::string target_n = input.front();
						input.pop();

						Player* target;

						if (target_n.compare("test_player") == 0) {
							target = this->GetTestPlayer();
						}
						else if(target_n.compare("player_1")) {
							target = this->GetPlayer(PLAYER_1);
						}
						else if (target_n.compare("player_2")) {
							target = this->GetPlayer(PLAYER_2);
						}
						else {
							std::cout << "Error: Invalid Target!" << std::endl;
							continue;
						}

						std::string anim = input.front();
						input.pop();

						int a = ((SkelMesh*)target->GetMesh())->GetSkeleton()->GetAnimByName(anim);
						if (a > -1) {
							((SkelMesh*)target->GetMesh())->SetAnim(a, 0);
							std::cout << target_n + " playing: " + anim << std::endl;
							continue;
						}
						else {
							std::cout << "Error: Animation not found." << std::endl;
							continue;
						}
					}
					else {
						std::cout << "Error: play requires 2 arguments: [target] [anim_name]" << std::endl;
						continue;
					}
				}
				else if (command.compare("exit") == 0) {
					break;
				}
				else {
					std::cout << "Error: Invalid Command." << std::endl;
					continue;
				}

			}
		}
	};

	threadObj = std::thread(console);
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
