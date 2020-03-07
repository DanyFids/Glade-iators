#include "PlayScenes.h"
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <random>
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
#include"Lerp.h"
#include "Sound.h"
#include "Game.h"
#include "PostProcess.h"
#include "Text.h"

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

	//this is test shit
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && time <= 0) {
		players[PLAYER_1]->dmgHP(10.0f);
		time = MAX_TIME;
	}
	//

	if (time > 0) {
		time -= dt;
	}
}

void OnePlayer::Update(float dt)
{

	audioEngine.Update();
	for (int c = 0; c < players.size(); c++) {
		players[c]->Update(dt);
		

		for (int p = 0; p < players.size(); p++) {
			if (players[c] != players[p]) {
				if (players[c]->HitDetect(players[p])) {
					std::cout << "Welp\n";
				}
			}
		}


		if (glfwJoystickPresent(c) && glfwJoystickIsGamepad(c)) {
			Cam[c]->Move(players[c]->phys.move, dt);

			Cam[c]->SetTarget(players[c]->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
		}

		players[c]->ApplyMove();
	}

	//for (int a = 0; a < attacks.size(); a++)
	//{
	//	attacks[a]->time -= dt;
	//	if (attacks[a]->time <= 0)
	//	{
	//		players[attacks[a]->getPlayer()]->DestroyChild(0);
	//		attacks.erase(attacks.begin() + a);
	//		break;
	//	}
	//	if (attacks[a]->HitDetect(players[0]) && attacks[a]->player != 0)
	//	{
	//		players[0]->dmgHP(10.0f);
	//		attacks[a]->Hit = true;
	//	}
	//	if (attacks[a]->HitDetect(players[1]) && attacks[a]->player != 1)
	//	{
	//		players[1]->dmgHP(10.0f);
	//		attacks[a]->Hit = true;
	//	}
	//}

	((MorphMesh*)(morphyBoi->GetMesh()))->Update(dt);
	// m morphyBoi->Update(dt);

	//for (int a = 0; a < shields.size(); a++)
	//{
	//	
	//	//if (shields[a]->player == 0 && block1 == false)
	//	//{
	//	//	shields.erase(shields.begin() + a);
	//	//	break;
	//	//}
	//	
	//}

	//DUUDE->Update(dt);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

	test_player->Update(dt);
}

void OnePlayer::Draw()
{
	main_pass->Clear();
	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->buff->Clear();
	}

	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Game::SCREEN.x, Game::SCREEN.y);
	
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		lights[l]->SetupDepthShader(skelDepth);
		RenderScene(depthShader, skelDepth);
		test_player->Draw(skelDepth, Cam);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, Game::SCREEN.x, Game::SCREEN.y);

		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(morphShader);
	sun->SetupLight(skelShader);
	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);
		lights[c]->SetupLight(skelShader, c);
	}
	shaderObj->SetI("num_lights", lights.size());
	morphShader->SetI("num_lights", lights.size());
	skelShader->SetI("num_lights", lights.size());

	/*shaderObj->SetB("enable_a", enable_ambient);
	shaderObj->SetB("enable_d", enable_diffuse);
	shaderObj->SetB("enable_s", enable_spec);
	shaderObj->SetB("enable_r", enable_rim);

	skelShader->SetB("enable_a", enable_ambient);
	skelShader->SetB("enable_d", enable_diffuse);
	skelShader->SetB("enable_s", enable_spec);
	skelShader->SetB("enable_r", enable_rim);*/

	main_pass->Use();

	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);
		Cam[c]->SetupCam(skelShader);

		RenderScene(shaderObj, skelShader);
		Cam[c]->SetupCam(morphShader);
		morphyBoi->Draw(morphShader, Cam);
		test_player->Draw(skelShader, Cam, shaderObj);
		
		glDisable(GL_DEPTH_TEST);
		//((SkelMesh*)(test_player->GetMesh()))->DrawSkeleton( test_player->GetTransform().GetWorldTransform(), shaderObj);
		glEnable(GL_DEPTH_TEST);

		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		players[0]->hitbox->Draw(shaderObj, players[PLAYER_1]->GetTransform().GetWorldTransform());
		players[1]->hitbox->Draw(shaderObj, players[PLAYER_2]->GetTransform().GetWorldTransform());
		test_player->hitbox->Draw(shaderObj, test_player->GetTransform().GetWorldTransform());



		weapons[0]->hitbox->Draw(shaderObj, weapons[0]->getParentTransform());
		shields[0]->hitbox->Draw(shaderObj, shields[0]->getParentTransform());
		//players[0]->GetTransform().GetWorldTransform() * weapons[0]->GetTransform().GetWorldTransform();
		//	players[1]->hitbox->Draw(shaderObj);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_DEPTH_TEST);
	}

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	if (post_pass.size() > 0) {
		glBindTexture(GL_TEXTURE_2D, post_pass.at(post_pass.size() - 1)->buff->GetOutput());
	}
	else {
		glBindTexture(GL_TEXTURE_2D, main_pass->GetOutput());
	}
	POST_OUT->SetI("INPUT", 0);

	Game::QUAD->Draw(POST_OUT);


	glDisable(GL_DEPTH_TEST);
	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	glEnable(GL_DEPTH_TEST);

	glViewport(0, 0, Game::SCREEN.x, Game::SCREEN.y);
}

void OnePlayer::LoadScene()
{
	main_pass = new FrameBuffer();

	// Post processing passes

	Material* LUT_TEST = new Material("LUTs/LUT_TEST.png");

	//post_pass.push_back(new LutColorCorrection(new LUT("LUTs/jungle.cube"), LUT_TEST->DIFF));
	//post_pass.push_back(new LutColorCorrection(new LUT("LUTs/Winterfell Extra 2.cube"), main_pass->GetOutput()));


	isMenu = false;
	ChangingScn = false;

	Joint::init();
	//{91f62782-35bd-42df-85a1-8f359308dd0c}

	//// Init AudioEngine (Don't forget to shut down and update)
	audioEngine.Init();

	//// Load a bank (Use the flag FMOD_STUDIO_LOAD_BANK_NORMAL)
	audioEngine.LoadBank("Master", FMOD_STUDIO_LOAD_BANK_NORMAL);

	//// Load an event
	audioEngine.LoadEvent("MenuPlaceholder", "{91f62782-35bd-42df-85a1-8f359308dd0c}");

	//// Play the event
	audioEngine.PlayEvent("MenuPlaceholder");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Basic_Shader.frag");
	skelDepth = new Shader("Shaders/depth_skel.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png");
	
	Material* GladiatorWM = new Material("WeightMap.png");

	Material* SnekWM = new Material("snek/WeightMap.png"); //Bone tester

	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");

	Material* arenaTex = new Material("wood_texture.png");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "bone_t.bvh");
	SkelMesh* GladiatorMesh = new SkelMesh("gladiator.obj", gladiatorSkel, "WeightMap.png");

	GladiatorMesh->SetAnim(gladiatorSkel->GetAnimByName("breathe"), 3, 0.34f, 0.5f);

	SkelMesh* P1_mesh = new SkelMesh(*GladiatorMesh);
	SkelMesh* P2_mesh = new SkelMesh(*GladiatorMesh);

	gladiatorSkel->WriteTree();
	//gladiatorSkel->Find("l_arm2")->animations[0][0].rotation += glm::vec3(0.0f, -90.0f, 0.0f);
	//gladiatorSkel->Find("l_arm1")->animations[0][0].scale = glm::vec3(2.0f, 2.0f, 2.0f);

	//wiggleSkel->Find("bone2")->animations[0][0].scale = glm::vec3(2.0f, 1.0f, 2.0f);

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");
	Mesh* arena = new Mesh("ColitreeumV2.obj");
	Mesh* sword_mesh = new Mesh("Weapons/Sword.obj");
	Mesh* shield_mesh = new Mesh("Weapons/Circle_Shield.obj");


	Hitbox* basicCubeHB = new CubeHitbox(1.2f,3.0f,1.2f);
	Hitbox* basicCubeHB2 = new CubeHitbox(1.0, 1.0f, 1.0f);

	//Capsule testing
	Hitbox* basicCapsuleHB = new CapsuleHitbox(0.4f,4.0f); //radius + height
	Hitbox* basicCapsuleHB2 = new CapsuleHitbox(0.8f,4.0f);
	Hitbox* basicCapsuleHB3 = new CapsuleHitbox(0.2,4.0);
	Hitbox* swordCapsuleHB = new CapsuleHitbox(0.09f, 12.8f);
	//Capsule Testing

	Hitbox* basicSphereHB = new SphereHitbox(1.0f);
	Hitbox* shieldSphereHB = new SphereHitbox(1.0f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);

	players.push_back(new Player(P1_mesh, defaultTex, basicCapsuleHB, { 4.0f, 0.0f, 0.0f })); // P1
	players.push_back(new Player(P2_mesh, D20Tex, basicCapsuleHB2)); //P2

	GladiatorMesh->SetAnim(1, 0);
	GladiatorMesh->SetFrame(0, 0);
	//players[PLAYER_1]->Rotate(glm::vec3(25, 0, 0));
	//shieldSphereHB->SetScale({0.2f, 1.0f, 0.1f});
	players[PLAYER_1]->dmgHP(50);

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ -6.0f, 0.0f, 0.0f });
	//players[PLAYER_2]->Rotate(glm::vec3(45,0,45));
	
	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });


	test_player = new Player(GladiatorMesh, defaultTex, basicCapsuleHB3, { 0.0f, 0.0f, 0.0f });
	//test_player->Scale(glm::vec3(1.2f));

	shieldSphereHB->SetScale(glm::vec3(0.1f, 0.65f, 0.65f));
	weapons.push_back(new Object(sword_mesh, defaultTex, swordCapsuleHB, glm::vec3(0.0f, 0.0f, 0.0f), gladiatorSkel->Find("r_hand"),P1_mesh));
	shields.push_back(new Object(shield_mesh, defaultTex, shieldSphereHB, glm::vec3(0.0f, 0.0f, 0.0f), gladiatorSkel->Find("l_hand"), P1_mesh));

	weapons[0]->SetPosition({-0.12f, 0.0f, -0.12f});
	weapons[0]->Scale({0.9f, 0.9f, 0.9f});
	weapons[0]->SetRotation({90.0f, 0.0f, 0.0f});
	

	shields[0]->SetPosition({ 0.095f, 0.115f, 0.0f });
	shields[0]->Scale({ 0.8f, 0.8f, 0.8f });
	shields[0]->SetRotation({ 0.0f, 0.0f, 270.0f });

	//shields[0]->hitbox->SetPosition(glm::vec3(0.0f, 0.3f, 0.0f));
	//glm::vec3 test = shields[0]->hitbox->GetTransform().position;
	players[PLAYER_1]->addChild(weapons[0]);
	players[PLAYER_1]->addChild(shields[0]);
	//sword->addChild(swordCapsuleHB);

	//test_bones = new Player(snekMesh, defaultTex, BlockyBoiHB, { -5.0f,0.0f,5.0f });
	//test_bones->Scale(glm::vec3(2.0f));

	GladiatorMesh->SetAnim(gladiatorSkel->GetAnimByName("walk"), 0, 1.0f, 0.5f);

	//players.push_back(new Player(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	//players[2]->Scale(glm::vec3(1.2f)); 

	Object* die = new Object(Square, DiceTex, basicCubeHB);
	die->Move({ 8.0f, 1.0f, -3.0f });
	terrain.push_back(die);

	////SAT Testing Stuff
	//Object* SATtest1 = new Object(Square, DiceTex, basicCubeHB);
	//die->Move({2.0f,1.0f,10.0f});
	//terrain.push_back(SATtest1);

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	Object* Colitreeum = new Object(arena, arenaTex, basicSphereHB, glm::vec3(0,-2,0));

	Colitreeum->Scale(glm::vec3(2.5, 2.5, 2.5));

	floor->Move({ 0.0f, -0.75f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	

	//terrain.push_back(floor);
	terrain.push_back(Colitreeum);

	beacons.push_back(glm::vec3(glm::vec3(1,1,1)));
	beacons.push_back(glm::vec3(glm::vec3(1,1,3)));
	beacons.push_back(glm::vec3(glm::vec3(1,2,5)));
	beacons.push_back(glm::vec3(glm::vec3(2,1,4)));
	beacons.push_back(glm::vec3(glm::vec3(3, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(7, 1, 4)));
	beacons.push_back(glm::vec3(glm::vec3(9, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(12, 1, 4)));
	beacons.push_back(glm::vec3(glm::vec3(4, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(2, 1, 4)));

	Mesh* BsMesh = new Mesh("d6.obj");
	Material* BsMat = new Material("missing_tex.png");

	//DUUDE = new SplineMan(BsMesh, BsMat, basicCubeHB, glm::vec3(1, 1, 1), beacons);

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, SCREEN_WIDTH, SCREEN_HEIGHT)) 
	};

	UI* hpBG = new UI(210, 30, { 5.0f, 545.0f, -1.0f }, blackBarMat);
	UI* stamBG = new UI(160, 30, { 5.0f, 495.0f, -1.0f }, blackBarMat);
	UI* crowdBG = new UI(185, 30, { 220.0f, 545.0f, -1.0f }, blackBarMat);

	UI* hpBG2 = new UI(210, 30, { 585.0f, 545.0f, -1.0f }, blackBarMat);
	UI* stamBG2 = new UI(160, 30, { 635.0f, 495.0f, -1.0f }, blackBarMat);
	UI* crowdBG2 = new UI(185, 30, { 395.0f, 545.0f, -1.0f }, blackBarMat);


	ui = {
		new HealthBar((Player*)players[PLAYER_1], glm::vec2(10, 550), hpBarMat, hpBG),
		new StaminaBar((Player*)players[PLAYER_1], glm::vec2(10, 500), stamBarMat, stamBG),
		new CrowdBar((Player*)players[PLAYER_1], glm::vec2(225, 550), crowdBarMat, crowdBG),

		//new HealthBar((Player*)players[PLAYER_1], glm::vec2(590, 550), hpBarMat, hpBG2),
		//new StaminaBar((Player*)players[PLAYER_1], glm::vec2(640, 500), stamBarMat, stamBG2),
		//new CrowdBar((Player*)players[PLAYER_1], glm::vec2(395, 550), crowdBarMat, crowdBG2)
	};
 

	std::vector<std::string> frames = { "wobble/wobble1.obj", "wobble/wobble2.obj", "wobble/wobble3.obj", "wobble/wobble4.obj" };

	morphyBoi = new Object(new MorphMesh(frames), defaultTex, basicCubeHB, glm::vec3(5.0f,1.0f,5.0f));
	//staticBoi = new Object(new Mesh("wobble/wobble2.obj"), defaultTex, basicCubeHB, glm::vec3(2.0f, 4.0f, 2.0f));

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

void OnePlayer::ResizeCams()
{
	Scene::ResizeCams();
	main_pass->Resize();

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->buff->Resize();
	}
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
	for (int c = 0; c < players.size(); c++) {
		players[c]->Update(dt);

		for (int p = 0; p < players.size(); p++) {
			if (players[c] != players[p]) {
				if (players[c]->HitDetect(players[p])) {
					std::cout << "Welp\n";
				}
			}
		}

		if (glfwJoystickPresent(c) && glfwJoystickIsGamepad(c)) {
			Cam[c]->Move(players[c]->phys.move, dt);

			players[c]->ApplyMove();

			Cam[c]->SetTarget(players[c]->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
		}
	}

	//for (int a = 0; a < attacks.size(); a++)
	//{
	//	attacks[a]->time -= dt;
	//	if (attacks[a]->time <= 0)
	//	{
	//		attacks.erase(attacks.begin() + a);
	//		break;
	//	}
	//	if (attacks[a]->HitDetect(players[0]) && attacks[a]->player != 0)
	//	{
	//		players[0]->dmgHP(10.0f);
	//		attacks[a]->Hit = true;
	//	}
	//	if (attacks[a]->HitDetect(players[1]) && attacks[a]->player != 1)
	//	{
	//		players[1]->dmgHP(10.0f);
	//		attacks[a]->Hit = true;
	//	}
	//}

	for (int a = 0; a < shields.size(); a++)
	{

		//if (shields[a]->player == 0 && block1 == false)
		//{
		//	shields.erase(shields.begin() + a);
		//	break;
		//}
		//if (shields[a]->player == 1 && block2 == false)
		//{
		//	shields.erase(shields.begin() + a);
		//	break;
		//}

	}

	DUUDE->Update(dt);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

	((MorphMesh*)(morphyBoi->GetMesh()))->Update(dt);
}

void TwoPlayer::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);

	RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		RenderScene(depthShader, depthShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	sun->SetupLight(shaderObj);
	sun->SetupLight(morphShader);
	sun->SetupLight(skelShader);

	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);
		lights[c]->SetupLight(skelShader, c);

	}
	shaderObj->SetI("num_lights", lights.size());
	morphShader->SetI("num_lights", lights.size());
	skelShader->SetI("num_lights", lights.size());


	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);
		Cam[c]->SetupCam(skelShader);

		RenderScene(shaderObj, skelShader);
		Cam[c]->SetupCam(morphShader);
		morphyBoi->Draw(morphShader, Cam);

		glDisable(GL_DEPTH_TEST);
		//players[0]->hitbox->Draw(shaderObj, players[PLAYER_1]->GetTransform().GetWorldTransform());
		//players[1]->hitbox->Draw(shaderObj, players[PLAYER_2]->GetTransform().GetWorldTransform());
		//	players[1]->hitbox->Draw(shaderObj);

		glEnable(GL_DEPTH_TEST);
	}

	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	glEnable(GL_DEPTH_TEST);


	
}

void TwoPlayer::LoadScene()
{
	isMenu = false;
	ChangingScn = false;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Basic_Shader.frag");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-normal.png");

	Material* arenaTex = new Material("wood_texture.png");

	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	beacons.push_back(glm::vec3(glm::vec3(1, 1, 1)));
	beacons.push_back(glm::vec3(glm::vec3(1, 1, 3)));
	beacons.push_back(glm::vec3(glm::vec3(1, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(2, 1, 4)));
	beacons.push_back(glm::vec3(glm::vec3(3, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(7, 1, 4)));
	beacons.push_back(glm::vec3(glm::vec3(9, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(12, 1, 4)));
	beacons.push_back(glm::vec3(glm::vec3(4, 2, 5)));
	beacons.push_back(glm::vec3(glm::vec3(2, 1, 4)));

	Mesh* BsMesh = new Mesh("d6.obj");
	Material* BsMat = new Material("missing_tex.png");

	DUUDE = new SplineMan(BsMesh, BsMat, basicCubeHB, glm::vec3(1, 1, 1), beacons);

	//sun = new DirectionalLight(glm::normalize(glm::vec3(1.5f, 1.0f, 0.5f)), { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f);
	//lights.push_back(new PointLight({ 0.0f, 30.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.014f, 0.0007f));
	//lights.push_back(new PointLight({ -4.0f, 1.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");

	Mesh* arena = new Mesh("ColitreeumV2.obj");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "bone_t.bvh");
	SkelMesh* P1_MESH = new SkelMesh("gladiator.obj", gladiatorSkel, "WeightMap.png");
	SkelMesh* P2_MESH = new SkelMesh(*P1_MESH);

	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);

	//Capsule testing
	Hitbox* basicCapsuleHB = new CapsuleHitbox(0.3f, 2.0f); //radius + height
	Hitbox* basicCapsuleHB2 = new CapsuleHitbox(0.3f, 2.0f);

	players.push_back(new Player(P1_MESH, defaultTex, basicCapsuleHB, { -3.0f, -0.6f, 0.0f })); // THIS IS PLAYER ONE
	players[PLAYER_1]->hitbox->parentTransform(players[PLAYER_1]->GetTransform());
	//players[PLAYER_1]->Rotate(glm::vec3(0.0f,90.0f,0.0f));
	players.push_back(new Player(P2_MESH, defaultTex, basicCapsuleHB2)); // THIS IS PLAYER TWO
	players[PLAYER_2]->hitbox->parentTransform(players[PLAYER_1]->GetTransform());

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, -0.6f, 0.0f });

	//terrain.push_back(new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	//terrain.b(glm::vec3(1.2f));

	Object* BOI = new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f });
	//floor->Move({ 0.0f, -1.5f, 0.0f });
	BOI->Scale(glm::vec3(1.2f));
	//terrain.push_back(BOI);
	//players[PLAYER_2]->Scale(glm::vec3(0.75f, 0.75f, 0.75f));

	Object* die = new Object(Square, DiceTex, basicCubeHB);
	die->Move({ 8.0f, 1.0f, 19.0f });
	terrain.push_back(die);

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	floor->Move({ 0.0f, -1.5f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	Object* Colitreeum = new Object(arena, arenaTex, basicSphereHB, glm::vec3(0, -2, 0));

	Colitreeum->Scale(glm::vec3(2.5, 2.5, 2.5));

	terrain.push_back(Colitreeum);

	//terrain.push_back(floor);

	Mesh* SwordMesh = new Mesh("Weapons/Sword.obj");
	Mesh* ShieldMesh = new Mesh("Weapons/Circle_Shield.obj");

	Hitbox* swordCapsuleHB = new CapsuleHitbox(0.09f, 12.8f);
	Hitbox* shieldSphereHB = new SphereHitbox(1.0f);
	shieldSphereHB->SetScale(glm::vec3(0.1f, 0.65f, 0.65f));

	Object* P1_sword = new Object(SwordMesh, defaultTex, swordCapsuleHB, { -0.12f, -0.04f, -0.27f }, gladiatorSkel->Find("r_hand"), P1_MESH);
	Object* P2_sword = new Object(SwordMesh, defaultTex, swordCapsuleHB, { -0.12f, -0.04f, -0.27f }, gladiatorSkel->Find("r_hand"), P2_MESH);

	Object* P1_shield = new Object(ShieldMesh, defaultTex, shieldSphereHB, { 0.095f, 0.115f, 0.0f }, gladiatorSkel->Find("l_hand"), P1_MESH);
	Object* P2_shield = new Object(ShieldMesh, defaultTex, shieldSphereHB, { 0.095f, 0.115f, 0.0f }, gladiatorSkel->Find("l_hand"), P2_MESH);

	P1_sword->Scale({ 0.9f, 0.9f, 0.9f });
	P1_sword->SetRotation({ 0.0f, 90.0f, 90.0f });

	P2_sword->Scale({ 0.9f, 0.9f, 0.9f });
	P2_sword->SetRotation({ 0.0f, 90.0f, 90.0f });

	P1_shield->Scale({ 0.8f, 0.8f, 0.8f });
	P1_shield->SetRotation({ 0.0f, 0.0f, 270.0f });

	P2_shield->Scale({ 0.8f, 0.8f, 0.8f });
	P2_shield->SetRotation({ 0.0f, 0.0f, 270.0f });

	players[PLAYER_1]->addChild(P1_sword);
	players[PLAYER_1]->addChild(P1_shield);

	players[PLAYER_2]->addChild(P2_sword);
	players[PLAYER_2]->addChild(P2_shield);


	Cam = {
		new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec4(0,0, SCREEN_WIDTH / 2, SCREEN_HEIGHT)), // Cam 1
		new Camera(glm::vec3(2.0f, 0.0f, -4.0f), glm::vec4(SCREEN_WIDTH / 2,0, SCREEN_WIDTH / 2, SCREEN_HEIGHT)) // Cam 2
	};

	UI* hpBG = new UI(210, 30, { 5.0f, 545.0f, -1.0f }, blackBarMat);
	UI* stamBG = new UI(160, 30, { 5.0f, 495.0f, -1.0f }, blackBarMat);
	UI* crowdBG = new UI(185, 30, { 220.0f, 545.0f, -1.0f }, blackBarMat);

	UI* hpBG2 = new UI(210, 30, { 585.0f, 545.0f, -1.0f }, blackBarMat);
	UI* stamBG2 = new UI(160, 30, { 635.0f, 495.0f, -1.0f }, blackBarMat);
	UI* crowdBG2 = new UI(185, 30, { 395.0f, 545.0f, -1.0f }, blackBarMat);


	ui = {
		new HealthBar((Player*)players[PLAYER_1], glm::vec2(10, 550), hpBarMat, hpBG),
		new StaminaBar((Player*)players[PLAYER_1], glm::vec2(10, 500), stamBarMat, stamBG),
		new CrowdBar((Player*)players[PLAYER_1], glm::vec2(225, 550), crowdBarMat, crowdBG),

		new HealthBar((Player*)players[PLAYER_2], glm::vec2(590, 550), hpBarMat, hpBG2),
		new StaminaBar((Player*)players[PLAYER_2], glm::vec2(640, 500), stamBarMat, stamBG2),
		new CrowdBar((Player*)players[PLAYER_2], glm::vec2(395, 550), crowdBarMat, crowdBG2)
	};

	std::vector<std::string> frames = { "wobble/wobble1.obj", "wobble/wobble2.obj", "wobble/wobble3.obj", "wobble/wobble4.obj" };
	morphyBoi = new Object(new MorphMesh(frames), defaultTex, basicCubeHB, glm::vec3(15.0f, 1.0f, 2.0f));

	players[PLAYER_1]->dmgHP(40);
	players[PLAYER_2]->dmgHP(60);
}

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

	//audioEngine.Update();

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

}

void MainMenu::Draw()
{
	glCullFace(GL_FRONT);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, sun->GetFrameBuffer());
	glClear(GL_DEPTH_BUFFER_BIT);
	sun->SetupDepthShader(sunShader);
	//sun->SetupDepthShader(skelDepth);

	RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		RenderScene(depthShader, sunShader);
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

		RenderScene(shaderObj, sunShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);
	}


	//glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}


	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(1.f, 1.f, 1.f));

	glEnable(GL_DEPTH_TEST);

}

void MainMenu::LoadScene()
{
	isMenu = true;
	ChangingScn = false;

	MAX_MENU = 0;
	MIN_MENU = -2;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	

	//Material* hpBarMat = new Material("yuck.png");
	//Material* stamBarMat = new Material("blue.png");
	//Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");
	Material* firstPlayer = new Material("redPlayer.png");
	Material* secondPlayer = new Material("bluePlayer.png");
	Material* buttonPlay = new Material("playButton.png");
	Material* buttonSettings = new Material("settingsButton.png");
	Material* buttonExit = new Material("exitButton.png");
	Material* buttonBlank = new Material("blankButton.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.0f, 0.0f, 0.0f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, SCREEN_WIDTH, SCREEN_HEIGHT))
	}; 

	playerOne = new ButtonSelect(0, glm::vec2(75, 245), firstPlayer);
	playerTwo = new ButtonSelect(1, glm::vec2(75, 245), secondPlayer);

	ui = {
		//new UI(SCREEN_WIDTH, SCREEN_HEIGHT, glm::vec3(0.0f), blackBarMat),
		playerOne,
		playerTwo,
		new Button(glm::vec2(80, 250), buttonPlay), 
		new Button(glm::vec2(80, 150), buttonSettings),
		new Button(glm::vec2(80, 50), buttonExit)
	};
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

	RenderScene(sunShader, sunShader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, sun->GetDepthMap());

	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		RenderScene(depthShader, sunShader);
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

		RenderScene(shaderObj, sunShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);

	}

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	glEnable(GL_DEPTH_TEST);

}

void CharacterC::LoadScene()
{
	isMenu = true;
	ChangingScn = false;

	MAX_MENU = 10;
	MIN_MENU = 7;

	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* firstPlayer = new Material("redPlayer.png");
	Material* secondPlayer = new Material("bluePlayer.png");
	Material* buttonPlay = new Material("readyButton.png");
	Material* buttonRandom = new Material("randomButton.png");

	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");

	playerOne = new ButtonSelect(0, glm::vec2(145, 230), firstPlayer);
	playerTwo = new ButtonSelect(1, glm::vec2(585, 230), secondPlayer);

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, SCREEN_WIDTH, SCREEN_HEIGHT))
	};

	ui = {
		playerOne,
		playerTwo,
		new Button(glm::vec2(150, 160), crowdBarMat),
		new Button(glm::vec2(150, 85), crowdBarMat),
		new Button(glm::vec2(590, 160), crowdBarMat),
		new Button(glm::vec2(590, 85), crowdBarMat),
		new Button(glm::vec2(100, 10), buttonPlay),
		new Button(glm::vec2(100, 235), buttonRandom),
		new Button(glm::vec2(540, 235), buttonRandom)
	};

	ui[0]->Resize(70, 70);
	ui[1]->Resize(70, 70);
	ui[2]->Resize(60, 60);
	ui[3]->Resize(60, 60);
	ui[4]->Resize(60, 60);
	ui[5]->Resize(60, 60);
	ui[6]->Resize(600, 60);
	ui[7]->Resize(140, 60);
	ui[8]->Resize(140, 60);
}