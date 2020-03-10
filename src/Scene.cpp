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
#include "Lerp.h"

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
	
	//if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && CgradeIDown[0] == false)
	//{
	//	if (CgradeI == 1)
	//	{
	//		CgradeI = 0;
	//	}
	//	else
	//		CgradeI = 1;
	//
	//}
	//if (glfwGetKey(window, GLFW_KEY_8) != GLFW_PRESS)
	//{
	//	CgradeIDown[0] == false;
	//}
	//if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && CgradeIDown[1] == false)
	//{								
	//	if (CgradeI == 2)
	//	{
	//		CgradeI = 0;
	//	}
	//	else
	//		CgradeI = 2;
	//}								
	//if (glfwGetKey(window, GLFW_KEY_9) != GLFW_PRESS)
	//{
	//	CgradeIDown[1] == false;
	//}
	//if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && CgradeIDown[2] == false)
	//{
	//	if (CgradeI == 3)
	//	{
	//		CgradeI = 0;
	//	}
	//	else
	//		CgradeI = 3;
	//}
	//if (glfwGetKey(window, GLFW_KEY_0) != GLFW_PRESS)
	//{
	//	CgradeIDown[2] = false;
	//}
	//if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	//{
	//	//attacks.push_back(new Attack(Amesh, Amat, basicCubeHB, glm::vec3(0,0,0), player));
	//	//glm::vec3 p1 = glm::vec3(0.0f, 0.0f, 0.0f);
	//	//p1.x += 2 * cos(glm::radians((players[0]->GetTransform().rotation.y)));
	//	//p1.z += 2 * -sin(glm::radians((players[0]->GetTransform().rotation.y)));
	//	//p1.y = players[0]->GetPosition().y;
	//	//attacks.back()->SetPosition(p1);
	//	//attacks.back()->ABox->SetPosition(glm::vec3(0,2,4));
	//}
	//if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS && dodge1 == false)
	//{
	//	players[player]->phys.move = m * (PLAYER_SPEED * 2) * dt;
	//	dodge1 = false;
	//	dodge1t = 3;
	//}
	//else if (m.x != 0.0f || m.y != 0.0f || m.z != 0.0f)
	//{
	//	players[player]->phys.move = m * PLAYER_SPEED * dt;
	//}
	//	
	//if (dodge1t <= 0)
	//{
	//	dodge1 = true;
	//}
	//

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

	static bool guardButton1 = false;
	static bool guardButton2 = false;


	GLFWgamepadstate state;
	if (glfwGetGamepadState(controller, &state)) {
		if (isMenu != true && !ChangingScn) {
			glm::vec2 rot = glm::vec2(0.0f, 0.0f);

			glm::vec3 player_head = players[player]->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f);
			if (!players[player]->GetCamLock()) {
				Cam[player]->SetPosition(player_head);
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

				Cam[player]->Spin(rot * Cam[player]->GetRotateSpeed() * dt);
			}
			else {
				unsigned int other = (player == PLAYER_1) ? PLAYER_2 : PLAYER_1;

				glm::vec3 mid = lerp(players[player]->GetPosition(), players[other]->GetPosition(), 0.5f) + glm::vec3(0.0f, 1.5f, 0.0f);
				glm::vec3 dir = glm::normalize((player_head + glm::vec3(0.0f, 1.0f, 0.0f)) - mid);

				Cam[player]->SetPosition(player_head + (dir * Cam[player]->GetRadius()) + glm::vec3(0.0f, 1.0f, 0.0f));
				Cam[player]->SetTarget(mid + glm::vec3(0.0f, 1.5f, 0.0f));
			}

			glm::vec3 t = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 yeet = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 camF = Cam[player]->GetDirection();
			glm::vec3 camR = Cam[player]->GetRight();

			glm::vec2 axisPos = glm::vec2(state.axes[GLFW_GAMEPAD_AXIS_LEFT_X], state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]);

			if (glm::length(axisPos) > dead_zone)
			{
				if (players[player]->GetFrameState() == FrameStates::Neutral || players[player]->GetFrameState() == FrameStates::Block) {
					t -= glm::normalize(glm::vec3(camF.x, 0.0f, camF.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
					//yeet.z = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
					t += glm::normalize(glm::vec3(camR.x, 0.0f, camR.z)) * state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
				}
				//yeet.x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
				if (players[player]->GetState() < PLAYER_STATE::walking && players[player]->GetFrameState() == FrameStates::Neutral && !players[player]->IsLocked()) {
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

				float newRot;

				newRot = -(std::atan2f(dir.z, dir.x)) * (180 / M_PI) + 90;

				//std::cout << newRot << std::endl;

				players[player]->SetRotation({ 0.0f, newRot, 0.0f });

				//std::cout << "move\n";
			}

			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
				((Player*)players[player])->Run();
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE) {
				((Player*)players[player])->StopRun();
			}

			static bool r_stick_down[2] = { false, false };
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_PRESS && !r_stick_down[player]) {
				players[player]->ToggleCamLock();
				r_stick_down[player] = true;
			}
			else if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_THUMB] == GLFW_RELEASE) {
				r_stick_down[player] = false;
			}

			static bool rb_p[2] = { false, false };
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS && !rb_p[player])
			{
				if (players[player]->GetStam() > 0.0f) {

					//glm::vec3 p1 = glm::vec3();
					//p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
					//p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));



					//players[player]->addChild(new Attack(Amesh, Amat, basicCubeHB, p1, ((SkelMesh*)players[player]->GetMesh())->GetSkeleton()->Find("l_arm1")));

					//std::cout << "OOF\n";
					//players[player]->dmgSTAM(15.0f);
					players[player]->Attack();
					atk1 = true;
					rb_p[player] = true;
				}
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_RELEASE)
			{
				atk1 = false;
				rb_p[player] = false;
			}

			if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS)
			{
				if (players[player]->GetStam() > 0.0f) {
					players[player]->Roll();
					dodge1 = false;
					dodge1t = 0.1;
				}
			}

			if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS) { //dylanote
				
				//block1 = true;

				//glm::vec3 p1 = glm::vec3();
				//p1.x += 1 * cos(glm::radians((players[player]->GetTransform().rotation.y)));
				//p1.z += 1 * -sin(glm::radians((players[player]->GetTransform().rotation.y)));

				//players[player]->addChild(new Shield(Amesh, Bmat, basicCubeHB, p1, player));
				players[player]->Block();
				
				switch (player) {
				case 0:
					guardButton1 = true;
					break;
				case 1:
					guardButton2 = true;
					break;
				}
				//std::cout << "Parry God\n";
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_RELEASE) {


				switch (player) {
				case 0:
					if (guardButton1) {
						players[player]->Idle();
						//players[player]->SetState();
						guardButton1 = false;
					}
					break;
				case 1:
					if (guardButton2) {
						players[player]->Idle();
						//players[player]->SetState();
						guardButton2 = false;
					}
					break;
				}

				//players[player]->DestroyChild(0);
			}
		}
		//MENU STUFF BELOW
		else
		{

		/*****************/
		/* Menu Movement */
		/*****************/
			if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > 0.2 && menu_time[controller] <= 0)
			{
				menuSpot[controller]--;
				if (controller == 0) {
					if (MAX_MENU == 0)
						playerOne->move(0, -100);
					else
						playerOne->move(0, -75);
				}
				else {
					if (MAX_MENU == 0)
						playerTwo->move(0, -100);
					else
						playerTwo->move(0, -75);
				}
				menu_time[controller] = MENU_TIME;
			}
			else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -0.2 && menu_time[controller] <= 0)
			{
				menuSpot[controller]++;
				if (controller == 0) {
					if (MAX_MENU == 0)
						playerOne->move(0, 100);
					else
						playerOne->move(0, 75);
				}
				else {
					if (MAX_MENU == 0)
						playerTwo->move(0, 100);
					else
						playerTwo->move(0, 75);
				}
				menu_time[controller] = MENU_TIME;
			}

			/************************/
			/* MENU SPOT CORRECTION */
			/************************/
			if (menuSpot[controller] < MIN_MENU)
			{
				menuSpot[controller] = MAX_MENU;
				if (!ChangingScn) {
					if (controller == 0) {
						playerOne->move(0, 300);
					}
					else {
						playerTwo->move(0, 300);
					}
				}
			}
			else if (menuSpot[controller] > MAX_MENU)
			{
				menuSpot[controller] = MIN_MENU;
				if (!ChangingScn) {
					if (controller == 0) {
						playerOne->move(0, -300);
					}
					else {
						playerTwo->move(0, -300);
					}
				}
			}

			/**********************/
			/* Anthony, Edit Here */
			/**********************/


			/************/
			/* Player 1 */
			/************/
			if (controller == 0) {
				switch (menuSpot[controller]) {
				case 10:
					//Random Button
					break;
				case 9:
					//Swords
					if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2 && menu_time[controller] <= 0) {
						weapon[controller]--;
						changeW[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2 && menu_time[controller] <= 0) {
						weapon[controller]++;
						changeW[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					break;
				case 8:
					//Shields
					if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2 && menu_time[controller] <= 0) {
						shield[controller]--;
						changeS[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2 && menu_time[controller] <= 0) {
						shield[controller]++;
						changeS[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					break;
				case 7:
					//Ready Button
					break;
				case 0:
					//Play Button
					break;
				case -1:
					//Settings Button
					break;
				case -2:
					//Exit Button
					break;
				}
				//std::cout << weapon[0] << std::endl;
				//IMAGE CHANGE
				if (weapon[controller] > MAX_W)
					weapon[controller] = MIN_W;
				if (weapon[controller] < MIN_W)
					weapon[controller] = MAX_W;

				if (shield[controller] > MAX_S)
					shield[controller] = MIN_S;
				if (shield[controller] < MIN_S)
					shield[controller] = MAX_S;

				if (changeW[controller]) {
					switch (weapon[controller]) {
					case 0:
						wOne->ChangeTex(swordIcon);
						wOne_p1->ChangeTex(daggerIcon);
						wOne_p2->ChangeTex(spearIcon);
						changeW[controller] = false;
						break;
					case 1:
						wOne->ChangeTex(spearIcon);
						wOne_p1->ChangeTex(swordIcon);
						wOne_p2->ChangeTex(hammerIcon);
						changeW[controller] = false;
						break;
					case 2:
						wOne->ChangeTex(hammerIcon);
						wOne_p1->ChangeTex(spearIcon);
						wOne_p2->ChangeTex(tridentIcon);
						changeW[controller] = false;
						break;
					case 3:
						wOne->ChangeTex(tridentIcon);
						wOne_p1->ChangeTex(hammerIcon);
						wOne_p2->ChangeTex(daggerIcon);
						changeW[controller] = false;
						break;
					case 4:
						wOne->ChangeTex(daggerIcon);
						wOne_p1->ChangeTex(tridentIcon);
						wOne_p2->ChangeTex(swordIcon);
						changeW[controller] = false;
						break;
					}
				}
				if (changeS[controller]) {
					switch (shield[controller]) {
					case 0:
						sOne->ChangeTex(shieldIcon);
						sOne_p1->ChangeTex(nothingIcon);
						sOne_p2->ChangeTex(bucklerIcon);
						changeS[controller] = false;
						break;
					case 1:
						sOne->ChangeTex(bucklerIcon);
						sOne_p1->ChangeTex(shieldIcon);
						sOne_p2->ChangeTex(nothingIcon);
						changeS[controller] = false;
						break;
					case 2:
						sOne->ChangeTex(nothingIcon);
						sOne_p1->ChangeTex(bucklerIcon);
						sOne_p2->ChangeTex(shieldIcon);
						changeS[controller] = false;
						break;
					}
				}
			}

			/************/
			/* Player 2 */
			/************/
			else {
				switch (menuSpot[controller]) {
				case 10:
					//Random Button
					break;
				case 9:
					//Swords
					if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2 && menu_time[controller] <= 0) {
						weapon[controller]--;
						changeW[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2 && menu_time[controller] <= 0) {
						weapon[controller]++;
						changeW[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					break;
				case 8:
					//Shields
					if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] < -0.2 && menu_time[controller] <= 0) {
						shield[controller]--;
						changeS[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					else if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_X] > 0.2 && menu_time[controller] <= 0) {
						shield[controller]++;
						changeS[controller] = true;
						menu_time[controller] = MENU_TIME;
					}
					break;
				case 7:
					//Ready Button
					break;
				case 0:
					//Play Button
					break;
				case -1:
					//Settings Button
					break;
				case -2:
					//Exit Button
					break;
				}

				//IMAGE CHANGE
				if (weapon[controller] > MAX_W)
					weapon[controller] = MIN_W;
				if (weapon[controller] < MIN_W)
					weapon[controller] = MAX_W;

				if (shield[controller] > MAX_S)
					shield[controller] = MIN_S;
				if (shield[controller] < MIN_S)
					shield[controller] = MAX_S;

				if (changeW[controller]) {
					switch (weapon[controller]) {
					case 0:
						wTwo->ChangeTex(swordIcon);
						wTwo_p1->ChangeTex(daggerIcon);
						wTwo_p2->ChangeTex(spearIcon);
						changeW[controller] = false;
						break;
					case 1:
						wTwo->ChangeTex(spearIcon);
						wTwo_p1->ChangeTex(swordIcon);
						wTwo_p2->ChangeTex(hammerIcon);
						changeW[controller] = false;
						break;
					case 2:
						wTwo->ChangeTex(hammerIcon);
						wTwo_p1->ChangeTex(spearIcon);
						wTwo_p2->ChangeTex(tridentIcon);
						changeW[controller] = false;
						break;
					case 3:
						wTwo->ChangeTex(tridentIcon);
						wTwo_p1->ChangeTex(hammerIcon);
						wTwo_p2->ChangeTex(daggerIcon);
						changeW[controller] = false;
						break;
					case 4:
						wTwo->ChangeTex(daggerIcon);
						wTwo_p1->ChangeTex(tridentIcon);
						wTwo_p2->ChangeTex(swordIcon);
						changeW[controller] = false;
						break;
					}
				}
				if (changeS[controller]) {
					switch (shield[controller]) {
					case 0:
						sTwo->ChangeTex(shieldIcon);
						sTwo_p1->ChangeTex(nothingIcon);
						sTwo_p2->ChangeTex(bucklerIcon);
						changeS[controller] = false;
						break;
					case 1:
						sTwo->ChangeTex(bucklerIcon);
						sTwo_p1->ChangeTex(shieldIcon);
						sTwo_p2->ChangeTex(nothingIcon);
						changeS[controller] = false;
						break;
					case 2:
						sTwo->ChangeTex(nothingIcon);
						sTwo_p1->ChangeTex(bucklerIcon);
						sTwo_p2->ChangeTex(shieldIcon);
						changeS[controller] = false;
						break;
					}
				}
			}
			
			/*********************/
			/* Button Management */
			/*         &         */
			/*  Scene Selection  */
			/*********************/
			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS && menu_time[controller] <= 0 && !ChangingScn) {
				_Abutton[controller] = true;
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS && menu_time[controller] <= 0 && !ChangingScn) {
				_Bbutton[controller] = true;
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_RELEASE && _Bbutton[controller] && !ChangingScn && MAX_MENU == 10) {
				_Bbutton[controller] = false;
				ChangingScn = true;
				isMenu = false;
				Game::CURRENT->setScene(SCENES::MAIN_MENU);
			}
			if (state.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_RELEASE && _Abutton[controller] && !ChangingScn) {
				_Abutton[controller] = false;
				//std::cout << menuSpot[controller] << std::endl;
				if (menuSpot[controller] == 7) {
					//Play
					ChangingScn = true;
					isMenu = false;
					Game::CURRENT->setScene(SCENES::PLAY_SCENE);
				}
				else if (menuSpot[controller] == 10) {
					//Randomize
				}
				else if (menuSpot[0] == 0) {
					//Character Scene
					ChangingScn = true;
					isMenu = false;
					Game::CURRENT->setScene(SCENES::CHARACTER_SCENE);
				}
				else if (menuSpot[0] == -1) {
					//Settings
				}
				else if (menuSpot[0] == -2) {
					//Exit
					//clean all Buffers & Shaders (destruct them) etc. (make full cleanup function)
					glfwSetWindowShouldClose(Game::CURRENT->GetWindow(), true);
				}
				menu_time[controller] = MENU_TIME;
			}
		}
	}


	menu_time[controller] -= dt;
}

bool PlayScene::loaded = false;

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
