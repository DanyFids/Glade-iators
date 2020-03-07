#include "Scene.h"
#include <iostream>

#include "Camera.h"
#include "Mesh.h"
#include "Object.h"
#include "Light.h"
#include "Constants.h"
#include"UI.h"
#include "Game.h"
#include "Skeleton.h"
#include "Shader.h"

#define _USE_MATH_DEFINES
#include <math.h>

void MenuScene::KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt)
{
}

void MenuScene::ControllerInput(unsigned int controller, int player, float dt)
{
}

void MenuScene::Update(float dt)
{
}

void MenuScene::Draw()
{
}

void MenuScene::LoadScene()
{
}

void MenuScene::InputHandle(GLFWwindow* window, glm::vec2 mousePos, float dt)
{
}


PlayScene::PlayScene()
{
	if (POST_OUT == nullptr) {
		POST_OUT = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Output.frag");
	}
}

void PlayScene::KeyboardInput(GLFWwindow* window, glm::vec2 mousePos, int player, float dt)
{
	float offsetX = ((float)mousePos.x - m_lastX) * MOUSE_SENSITIVITY;
	float offsetY = (m_lastY - (float)mousePos.y) * MOUSE_SENSITIVITY;
	m_lastX = (float)mousePos.x;
	m_lastY = (float)mousePos.y;

	if ((offsetX != 0.0f || offsetY != 0.0f) && !firstMouse) {
		Cam[player]->Rotate(glm::vec2(offsetX, offsetY));
	}
	else if ((offsetX != 0.0f || offsetY != 0.0f) && firstMouse) {
		firstMouse = false;
	}

	glm::vec3 m = glm::vec3(0.0f, 0.0f, 0.0f);

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		m -= glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		m += glm::vec3(0.0f, 0.0f, 1.0f);
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		m -= glm::vec3(1.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		m += glm::vec3(1.0f, 0.0f, 0.0f);
	

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		//attacks.push_back(new Attack(Amesh, Amat, basicCubeHB, glm::vec3(0,0,0), player));
		//glm::vec3 p1 = glm::vec3(0.0f, 0.0f, 0.0f);
		//p1.x += 2 * cos(glm::radians((players[0]->GetTransform().rotation.y)));
		//p1.z += 2 * -sin(glm::radians((players[0]->GetTransform().rotation.y)));
		//p1.y = players[0]->GetPosition().y;
		//attacks.back()->SetPosition(p1);
		//attacks.back()->ABox->SetPosition(glm::vec3(0,2,4));
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && dodge1 == false)
	{
		players[player]->phys.move = m * (PLAYER_SPEED * 2) * dt;
		dodge1 = false;
		dodge1t = 3;
	}
	else if (m.x != 0.0f || m.y != 0.0f || m.z != 0.0f)
	{
		players[player]->phys.move = m * PLAYER_SPEED * dt;
	}
		
	if (dodge1t <= 0)
	{
		dodge1 = true;
	}


	glm::vec3 t = glm::vec3(0.0f, 0.0f, 0.0f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		t += Cam[player]->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		t -= Cam[player]->GetDirection();
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		t -= Cam[player]->GetRight();
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		t += Cam[player]->GetRight();
	if (t.x != 0.0f || t.y != 0.0f || t.z != 0.0f)
		Cam[player]->Move(glm::normalize(t), dt);
}

void PlayScene::ControllerInput(unsigned int controller, int player, float dt)
{
	static const float dead_zone = 0.35f;

	GLFWgamepadstate state;
	if (glfwGetGamepadState(controller, &state)) {
		if (isMenu != true && !ChangingScn) {
			glm::vec2 rot = glm::vec2(0.0f, 0.0f);
			if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < -0.2) {
				rot.y = -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
			}
			else {
				rot.y = 0.0f;
			}
			if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -0.2) {
				rot.x = -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
			}
			else {
				rot.x = 0.0f;
			}

			glm::vec3 t = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 yeet = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 camF = Cam[player]->GetDirection();
			glm::vec3 camR = Cam[player]->GetRight();

			glm::vec2 axisPos = glm::vec2(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);

			if (glm::length(axisPos) > dead_zone)
			{
				t -= glm::normalize(glm::vec3(camF.x, 0.0f, camF.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				//yeet.z = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				t += glm::normalize(glm::vec3(camR.x, 0.0f, camR.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				//yeet.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				if (players[player]->GetState() != PLAYER_STATE::walking && !players[player]->IsLocked()) {
					players[player]->PlayAnim("walk", 0, glm::length(axisPos));
					//players[player]->PlayAnim("idle", 1, 1.0f - glm::length(axisPos));
					players[player]->SetState(walking);
				}
				else if (!players[player]->IsLocked()) {
					((SkelMesh*)players[player]->GetMesh())->SetIntensity(0, glm::length(axisPos));
					//((SkelMesh*)players[player]->GetMesh())->SetIntensity(1, 1.0f - glm::length(axisPos));
				}
			}
			else if (players[player]->GetState() == PLAYER_STATE::walking) {
				players[player]->Idle();
			}

			if (t.x != 0.0f || t.y != 0.0f || t.z != 0.0f) {
				//players[player]->phys.move = glm::normalize(t) * 10.f * dt;

				glm::vec3 dir = glm::normalize(t);

				glm::vec3 t = glm::vec3(0.0f, 0.0f, 0.0f);
				glm::vec3 yeet = glm::vec3(0.0f, 0.0f, 0.0f);
				glm::vec3 camF = Cam[player]->GetDirection();
				glm::vec3 camR = Cam[player]->GetRight();
				if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.2)
				{
					t -= glm::normalize(glm::vec3(camF.x, 0.0f, camF.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
					//yeet.z = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				}

				if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2)
				{
					t += glm::normalize(glm::vec3(camR.x, 0.0f, camR.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
					//yeet.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				}

				if (t.x != 0.0f || t.y != 0.0f || t.z != 0.0f) {
					players[player]->phys.move = glm::normalize(t) * 10.f * dt;

					glm::vec3 dir = glm::normalize(t);

					float newRot;

					if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
						((Player*)players[player])->Run();
					}
					if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE) {
						((Player*)players[player])->StopRun();
					}

					if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.2)
					{
						if (players[player]->GetStam() >= 15.0f) {

							glm::vec3 p1 = glm::vec3();
							p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
							p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));

							players[player]->addChild(new Shield(Amesh, Bmat, basicCubeHB, p1, player));
						}
						if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE && player == PLAYER_1 && block1 == true) {

							block1 = false;
							players[player]->DestroyChild(0);
						}
					}
					if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] < 0.2)
					{
						atk1 = false;
					}

					if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS)
					{
						if (players[player]->GetStam() >= 20.0f) {
							players[player]->Roll();
							dodge1 = false;
							dodge1t = 0.1;
						}
					}

					if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS) { //dylanote
						std::cout << "Parry God\n";
						block1 = true;
						glm::vec3 p1 = glm::vec3();
						p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
						p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));

						//players[player]->addChild(new Shield(Amesh, Bmat, basicCubeHB, p1, player));
					}
					if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE) {

						block1 = false;
						//players[player]->DestroyChild(0);
					}
				}

				menu_time[controller] -= dt;
			}
		}
	}
}

void PlayScene::RenderScene(Shader* shader, Shader* playerShader)
{
	for (int c = 0; c < players.size(); c++) {
		players[c]->Draw(playerShader, Cam, shader);
	}

	for (int t = 0; t < terrain.size(); t++) {
		terrain[t]->Draw(shader, Cam);
	}

	
}

void Scene::ResizeCams()
{
	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->UpdateScreen({ Game::SCREEN.x / Cam.size() * c,0, Game::SCREEN.x / Cam.size(),Game::SCREEN.y});
	}
}

Shader* PlayScene::POST_OUT = nullptr;
