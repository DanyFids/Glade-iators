#include "Scene.h"
#include <iostream>

#include"Camera.h"
#include"Mesh.h"
#include"Object.h"
#include"Light.h"
#include "Constants.h"
#include"UI.h"

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
		attacks.push_back(new Attack(Amesh, Amat, basicCubeHB, glm::vec3(0,0,0), player));
		glm::vec3 p1 = glm::vec3(0.0f, 0.0f, 0.0f);
		p1.x += 2 * cos(glm::radians((players[0]->GetTransform().rotation.y)));
		p1.z += 2 * -sin(glm::radians((players[0]->GetTransform().rotation.y)));
		p1.y = players[0]->GetPosition().y;
		attacks.back()->SetPosition(p1);
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
	GLFWgamepadstate state;
	if (glfwGetGamepadState(controller, &state)) {
		glm::vec2 rot = glm::vec2(0.0f, 0.0f);
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y] < -0.2) {
			rot.y = -state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y];
		}
		else {
			rot.y = 0.0f;
		}
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] > 0.2 || state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X] < -0.2) {
			rot.x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X];
		}
		else {
			rot.x = 0.0f;
		}
		Cam[player]->Spin(rot * Cam[player]->GetRotateSpeed() * dt);

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

			newRot = -(std::atan2f(dir.z, dir.x)) * (180/M_PI);

			std::cout << newRot << std::endl;

			players[player]->SetRotation({ 0.0f, newRot, 0.0f });

			std::cout << "move\n";
		}

		if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && player == PLAYER_1) {
			((Player*)players[player])->Run();
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE && player == PLAYER_1) {
			((Player*)players[player])->StopRun();
		}
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.2 && player == PLAYER_1 && atk1 == false)
		{
			attacks.push_back(new Attack(Amesh, Amat, basicCubeHB, glm::vec3(0, 0, 0), PLAYER_1));
			glm::vec3 p1 = players[player]->GetPosition();
			p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.y = players[player]->GetPosition().y;
			attacks.back()->SetPosition(p1);
			std::cout << "OOF\n";
			players[player]->dmgSTAM(15.0f);
			atk1 = true;
		}
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] < 0.2 && player == PLAYER_1)
		{
			atk1 = false;
		}

		if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && player == PLAYER_1 && dodge1 == true && t != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			players[player]->phys.move = t * (PLAYER_SPEED * 6) * dt;
			std::cout << "Dodgy boi\n";
			
			players[player]->dmgSTAM(20.0f);
			dodge1 = false;
			dodge1t = 0.1;
		}
		
		if (dodge1t <= -0.4 && dodge1 == false && player == PLAYER_1)
		{
			dodge1 = true;
		}
		else if (dodge1 == false && dodge1t >= 0 && player == PLAYER_1)
		{
			players[player]->phys.move = t * (PLAYER_SPEED * 6) * dt;
			dodge1t -= dt;
		}
		else
		{
			dodge1t -= dt;
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && player == PLAYER_1 && block1 == false) {
			std::cout << "Parry God\n";
			block1 = true;
			shields.push_back(new Shield(Amesh, Bmat, basicCubeHB, glm::vec3(0, 0, 0), player));
			glm::vec3 p1 = players[player]->GetPosition();
			p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.y = players[player]->GetPosition().y;
			shields.back()->SetPosition(p1);
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE && player == PLAYER_1) {

			block1 = false;
		}

		////////////////////////////PLAYER 2

		if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && player == PLAYER_2) {
			((Player*)players[player])->Run();
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE && player == PLAYER_2) {
			((Player*)players[player])->StopRun();
		}
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] > 0.2 && player == PLAYER_2 && atk2 == false)
		{
			attacks.push_back(new Attack(Amesh, Amat, basicCubeHB, glm::vec3(0, 0, 0), PLAYER_2));
			glm::vec3 p1 = players[player]->GetPosition();
			p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.y = players[player]->GetPosition().y;
			attacks.back()->SetPosition(p1);
			players[player]->dmgSTAM(15.0f);
			std::cout << "OOF\n";
			atk2 = true;
		}
		if (state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] < 0.2 && player == PLAYER_2)
		{
			atk2 = false;
		}

		if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && player == PLAYER_2 && dodge2 == true && t != glm::vec3(0.0f, 0.0f, 0.0f))
		{
			players[player]->phys.move = t * (PLAYER_SPEED * 6) * dt;
			std::cout << "Dodgy boi\n";

			players[player]->dmgSTAM(20.0f);
			dodge2 = false;
			dodge2t = 0.1;
		}

		if (dodge2t <= -0.4 && dodge2 == false && player == PLAYER_2)
		{
			dodge2 = true;
		}
		else if (dodge2 == false && dodge2t >= 0 && player == PLAYER_2)
		{
			players[player]->phys.move = t * (PLAYER_SPEED * 6) * dt;
			dodge2t -= dt;
		}
		else
		{
			dodge2t -= dt;
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS && player == PLAYER_2 && block2 == false) {
			std::cout << "Parry God\n";
			block2 = true;
			shields.push_back(new Shield(Amesh, Bmat, basicCubeHB, glm::vec3(0, 0, 0), player));
			glm::vec3 p1 = players[player]->GetPosition();
			p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));
			p1.y = players[player]->GetPosition().y;
			shields.back()->SetPosition(p1);
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE && player == PLAYER_2) {

			block2 = false;
		}
		
		if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && player == PLAYER_2 && Target2 == false) {
			std::cout << "Look at me\n";
			//glm::vec3 direction = glm::normalize(players[0]->GetPosition() - players[1]->GetPosition());
			Target2 == true;
		}
		if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_RELEASE && player == PLAYER_2) {

			Target2 = false;
		}
	}
}

void PlayScene::RenderScene(Shader* shader)
{
	for (int c = 0; c < players.size(); c++) {
		players[c]->Draw(shader, Cam);
	}

	for (int t = 0; t < terrain.size(); t++) {
		terrain[t]->Draw(shader, Cam);
	}

	for (int a = 0; a < attacks.size(); a++)
	{
		attacks[a]->Draw(shader, Cam);

	}

	for (int s = 0; s < shields.size(); s++)
	{
		shields[s]->Draw(shader, Cam);

	}

	DUUDE->Draw(shader, Cam);
}
