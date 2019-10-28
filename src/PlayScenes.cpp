#include "PlayScenes.h"
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <iostream>
#include <stdlib.h>

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

void OnePlayer::KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt)
{
	static glm::vec3 m = glm::vec3(0.0f, 0.0f, 0.0f);
	
	if (bodyParts.size() > 0) {
		bodyParts[0]->setLast(m); //find the last position before m actually changes to the new direction
	}
	for (int b = 1; b < bodyParts.size(); b++) {
		int l = b - 1;
		bodyParts[b]->setLast(bodyParts[l]->getLast()); //find the last positions of other body parts
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m = glm::vec3(0.0f, 0.0f, -1.0f);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m = glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m = glm::vec3(-1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m = glm::vec3(1.0f, 0.0f, 0.0f);
	if (m.x != 0.0f || m.y != 0.0f || m.z != 0.0f) {
		//players[PLAYER_1]->setLast(players[PLAYER_1]->GetPosition());
		players[PLAYER_1]->Move(m * PLAYER_SPEED * dt);
		for (int b = 0; b < bodyParts.size(); b++) {
			bodyParts[b]->Move(bodyParts[b]->getLast() * PLAYER_SPEED * dt); // move body in the past directions to generate a follow

			if (players[PLAYER_1]->hitbox->HitDetect(players[PLAYER_1]->GetTransform(), (CubeHitbox*)(bodyParts[b]->hitbox), bodyParts[b]->GetTransform())) {
				players[PLAYER_1]->setPos(glm::vec3(0.0f, 0.0f, 0.0f)); //body part collision resets positions
			}
		}

		for (int i = 0; i < terrain.size(); i++) {
			if (players[PLAYER_1]->hitbox->HitDetect(players[PLAYER_1]->GetTransform(), (CubeHitbox*)(terrain[i]->hitbox), terrain[i]->GetTransform())) {
				//std::cout << ("HIT DETECTED");
				if (!terrain[i]->getPellet()) {
					//This makes player stop when touching an object/wall
					//players[PLAYER_1]->Move(m * -1.0f * PLAYER_SPEED * dt);
					//resets player when touching object
					players[PLAYER_1]->setPos(glm::vec3(0.0f, 0.0f, 0.0f)); // wall collision resets position

					//bodyParts.clear(bodyParts.size());
					//glDeleteVertexArrays
					//This is where the body parts would delete(when you die)
				} 
				else {

					glm::vec3 n = glm::vec3(0.0f, 0.6f, 0.0f);
					float random1 = (rand() % (10 - (-10))) + (-10);
					float random2 = (rand() % (7 - (-7))) + (-7);
					n += glm::vec3(random1, 0.0f, random2);
					terrain[i]->setPos(n);
					
					Material* defaultTex = new Material("default-texture.png", "default-texture.png");
					Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);
					Mesh* Square = new Mesh("d6.obj");
					Object* player = new Object(Square, defaultTex, basicCubeHB);
					player->Move({ players[PLAYER_1]->GetPosition() });
					player->Scale({ 0.75f,0.75f,0.75f });
					player->setBody(true);
					
					bodyParts.push_back(player);

				}
			}
		}
	}
}

void OnePlayer::Update(float dt)
{
	
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

void OnePlayer::LoadScene()
{
	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 25.0f, 0.5f)), { 1.0f, 1.0f, 1.0f }, 0.1f, 0.2f, 0.2f);
	//lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 3.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");

	Hitbox* basicCubeHB = new CubeHitbox(1.0f,1.0f,1.0f);
	players.push_back(new Object(Square, DiceTex, basicCubeHB));
	players[PLAYER_1]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_1]->Move({ 0.0f, 0.3f, 0.0f });

	Object* pellet = new Object(Square, defaultTex, basicCubeHB);

	float random1 = (rand() % (10 - (-10))) + (-10);
	float random2 = (rand() % (7 - (-7))) + (-7);
	pellet->Move({ random1, 0.6f, random2 });
	pellet->Scale({ 0.6f, 0.6f, 0.6f });
	pellet->setPellet(true);

	terrain.push_back(pellet);

	Object* botWall = new Object(Square, defaultTex, basicCubeHB);
	botWall->Move({ 0.0f, 1.0f, -9.5f });
	botWall->Scale({ 30.0f, 2.0f, 2.0f });

	terrain.push_back(botWall);

	Object* topWall = new Object(Square, defaultTex, basicCubeHB);
	topWall->Move({ 0.0f, 1.0f, 9.0f });
	topWall->Scale({ 30.0f, 2.0f, 2.0f });

	terrain.push_back(topWall);

	Object* rightWall = new Object(Square, defaultTex, basicCubeHB);
	rightWall->Move({ 12.0f, 1.0f, 0.0f });
	rightWall->Scale({ 2.0f, 2.0f, 30.0f });

	terrain.push_back(rightWall);

	Object* leftWall = new Object(Square, defaultTex, basicCubeHB);
	leftWall->Move({ -12.0f, 1.0f, 0.0f });
	leftWall->Scale({ 2.0f, 2.0f, 30.0f });

	terrain.push_back(leftWall);

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	floor->Move({ 0.0f, -0.75f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	terrain.push_back(floor);

	Cam = {
		new Camera(glm::vec3(0.0f, 23.0f, 5.0f), {0.0f, 0.0f, 0.0f}, glm::vec4(0,0, SCREEN_WIDTH, SCREEN_HEIGHT))
	};

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
	Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");

	//sun = new DirectionalLight(glm::normalize(glm::vec3(1.5f, 1.0f, 0.5f)), { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f);
	lights.push_back(new PointLight({ 0.0f, 30.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.014f, 0.0007f));
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
