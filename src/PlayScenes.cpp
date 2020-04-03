#include "PlayScenes.h"
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
//#include <florp/app/timing.h>

#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Light.h"
//#include "Object.h"
#include "Mesh.h"
#include "Constants.h"
#include "Test_Primitives.h"
#include "Hitbox.h"
#include "UI.h"
#include "Skeleton.h"
#include "Lerp.h"
#include "Sound.h"
#include "Game.h"
#include "PostProcess.h"
#include "Text.h"
#include "Particle.h"

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

	// CG INPUTS
	static bool plus_p = false;
	if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && !plus_p) {
		if (active_lights < MAX_LIGHTS) {
			active_lights++;
			plus_p = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_RELEASE) {
		plus_p = false;
	}
	
	static bool minus_p = false;
	if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS && !minus_p) {
		if (active_lights > 1) {
			active_lights--;
			minus_p = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_RELEASE) {
		minus_p = false;
	}

	static bool p_1 = false;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !p_1) {
		active_lights = 1;
		toggle = CG_OUTPUT;
		p_1 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		p_1 = false;
	}

	static bool p_2 = false;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !p_2) {
		show_volumes = !show_volumes;
		p_2 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		p_2 = false;
	}

	static bool p_3 = false;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !p_3) {
		toggle = CG_DEPTH;
		p_3 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
		p_3 = false;
	}

	static bool p_4 = false;
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !p_4) {
		toggle = CG_NORMALS;
		p_4 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) {
		p_4 = false;
	}

	static bool p_5 = false;
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !p_5) {
		toggle = CG_COLOR;
		p_5 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) {
		p_5 = false;
	}

	static bool p_6 = false;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !p_6) {
		toggle = CG_LIGHTS;
		p_6 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) {
		p_6 = false;
	}

	static bool p_7 = false;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !p_7) {
		if (PointLight::VOLUME == PointLight::SPHERE)
			PointLight::VOLUME = PointLight::CUBE;
		else
			PointLight::VOLUME = PointLight::SPHERE;
		p_7 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) {
		p_7 = false;
	}

	static bool p_8 = false;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && !p_8) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_hot, post_pass.at(0)->buff->GetOutput()));

		p_8 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) {
		p_8 = false;
	}

	static bool p_9 = false;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !p_9) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_cool, post_pass.at(0)->buff->GetOutput()));

		p_9 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
		p_9 = false;
	}

	static bool p_0 = false;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !p_0) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_custom, post_pass.at(0)->buff->GetOutput()));

		p_0 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
		p_0 = false;
	}

	static bool p_o = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS && !p_o) {
		move_lights = !move_lights;
		p_o = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
		p_o = false;
	}

	static bool p_p = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !p_p) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		p_p = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		p_p = false;
	}

	static bool p_lb = false;
	if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_PRESS && !p_lb) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}

		post_pass.push_back(new PostProcess({ post_pass.at(0)->buff->GetOutput() }, highlightshader));
		post_pass.push_back(new PostProcess({ post_pass.at(1)->buff->GetOutput() }, horgausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(2)->buff->GetOutput() }, vergausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(3)->buff->GetOutput(),post_pass.at(0)->buff->GetOutput() }, bloomshader));
		post_pass.push_back(new PostProcess({ post_pass.at(4)->buff->GetOutput() }, highlightshader));
		post_pass.push_back(new PostProcess({ post_pass.at(5)->buff->GetOutput() }, horgausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(6)->buff->GetOutput() }, vergausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(7)->buff->GetOutput(),post_pass.at(0)->buff->GetOutput() }, bloomshader));

		p_lb = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT_BRACKET) == GLFW_RELEASE) {
		p_lb = false;
	}

	static bool p_rb = false;
	if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET ) == GLFW_PRESS && !p_rb) {
		toggle = CG_OUTPUT;
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new PostProcess({ post_pass.at(0)->buff->GetOutput() }, pixelshader));
		p_rb = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT_BRACKET) == GLFW_RELEASE) {
		p_rb = false;
	}

	if (time > 0) {
		time -= dt;
	}
}

void OnePlayer::Update(float dt)
{

	if (players[0]->GetHP() <= 0 || players[1]->GetHP() <= 0)
	{
		deathtimer -= dt;
		if (deathtimer <= 0)
		{
			Game::CURRENT->setScene(SCENES::MAIN_MENU);
			//setScene(MainMenu);

		}
	}

	
	for (int c = 0; c < players.size(); c++) {
		players[c]->Update(dt);
		

		for (int p = 0; p < players.size(); p++) {
			if (players[c] != players[p]) {
				if (players[c]->HitDetect(players[p])) {
					std::cout << "Welp\n";
				}
			}
		}

		if (move_lights) {
			for (int l = 0; l < active_lights && l < lights.size(); l++) {
				glm::vec3 tmp(0.0f);
				tmp.x = ((((float)(rand() % 2) / 2.0f)) * dt);
				tmp.z = ((((float)(rand() % 2) / 2.0f)) * dt);

				lights[l]->Move(tmp);
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

	fire->Update(dt);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec4("indexColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
	skelShader->SetVec4("indexColor", glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	test_player->Update(dt);
}

void OnePlayer::Draw()
{
	ClearBuffs();

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

	for (int l = 0; l < active_lights && l <  lights.size(); l++) {
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
	for (int c = 0; c < active_lights && c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);
		lights[c]->SetupLight(skelShader, c);
		lights[c]->SetupLight(lightPass, c);
	}
	shaderObj->SetI("num_lights", lights.size());
	morphShader->SetI("num_lights", lights.size());
	skelShader->SetI("num_lights", lights.size());
	lightPass->SetI("num_lights", active_lights);

	/*shaderObj->SetB("enable_a", enable_ambient);
	shaderObj->SetB("enable_d", enable_diffuse);
	shaderObj->SetB("enable_s", enable_spec);
	shaderObj->SetB("enable_r", enable_rim);

	skelShader->SetB("enable_a", enable_ambient);
	skelShader->SetB("enable_d", enable_diffuse);
	skelShader->SetB("enable_s", enable_spec);
	skelShader->SetB("enable_r", enable_rim);*/

	main_pass[0]->Use();

	for (int c = 0; c < Cam.size(); c++) {
		Cam[c]->SetupCam(shaderObj);
		Cam[c]->SetupCam(skelShader);

		RenderScene(shaderObj, skelShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);
		test_player->Draw(skelShader, Cam, shaderObj);

		//glDisable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		
		//players[0]->hitbox->Draw(shaderObj, players[PLAYER_1]->GetTransform().GetWorldTransform());
		//players[1]->hitbox->Draw(shaderObj, players[PLAYER_2]->GetTransform().GetWorldTransform());
		//test_player->hitbox->Draw(shaderObj, test_player->GetTransform().GetWorldTransform());
		//
		//weapons[0]->hitbox->Draw(shaderObj, weapons[0]->getParentTransform());
		//shields[0]->hitbox->Draw(shaderObj, shields[0]->getParentTransform());
		//players[0]->GetTransform().GetWorldTransform() * weapons[0]->GetTransform().GetWorldTransform();
		//	players[1]->hitbox->Draw(shaderObj);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glEnable(GL_DEPTH_TEST);

		Cam[c]->SetupPostLight(lightPass, c);
	}

	lightPass->SetI("num_cams", Cam.size());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetOutput(1));
	lightPass->SetI("normals", 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetDepth());
	lightPass->SetI("depth", 1);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetOutput(2));
	lightPass->SetI("spec", 2);

	light_buff[0]->Use();

	Game::QUAD->Draw(lightPass);

	particle_buff[0]->Use();
	glDisable(GL_DEPTH_TEST);
	((SkelMesh*)(test_player->GetMesh()))->DrawSkeleton(test_player->GetTransform().GetWorldTransform(), shaderObj);
	glEnable(GL_DEPTH_TEST);

	for (int c = 0; c < Cam.size(); c++) {
		fire->Draw(Cam[c]);
	}

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->Draw();
	}

	//SCREEN_WIDTH, SCREEN_HEIGHT

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);

	switch (toggle) {
	case CG_DEPTH:
		glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetDepth());
		break;
	case CG_LIGHTS:
		glBindTexture(GL_TEXTURE_2D, light_buff[0]->GetOutput());
		break;
	case CG_COLOR:
		glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetOutput());
		break;
	case CG_NORMALS:
		glBindTexture(GL_TEXTURE_2D, main_pass[0]->GetOutput(1));
		break;
	case CG_OUTPUT:
		if (post_pass.size() > 0) {
			glBindTexture(GL_TEXTURE_2D, post_pass.at(post_pass.size() - 1)->buff->GetOutput());
		}
		else {
			glBindTexture(GL_TEXTURE_2D, light_buff[0]->GetOutput(0));
		}
		break;
	}
	POST_OUT->SetI("INPUT", 0);

	Game::QUAD->Draw(POST_OUT);


	glDisable(GL_DEPTH_TEST);
	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	if (show_volumes) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int c = 0; c < active_lights && c < lights.size(); c++) {
			lights[c]->Draw(shaderObj);
		}
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if (players[0]->GetHP() <= 0 && players[1]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Draw", SCREEN_HEIGHT /2.f, SCREEN_WIDTH /2.f, 1.0f, glm::vec3(1.f, 0.f, 0.f));
		//deathtimer -= 
	}
	else if (players[0]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Player 1 Wins", SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, 1.0, glm::vec3(1.f, 0.f, 0.f));

	}
	else if (players[1]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Player 2 Wins", SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, 1.0, glm::vec3(1.f, 0.f, 0.f));

	}
	glEnable(GL_DEPTH_TEST);

	
	glViewport(0, 0, Game::SCREEN.x, Game::SCREEN.y);
}

void OnePlayer::LoadScene()
{
	main_pass[0] = new FrameBuffer();
	main_pass[0]->AddComponent();
	main_pass[0]->AddComponent();

	light_buff[0] = new FrameBuffer();

	particle_buff[0] = new FrameBuffer(main_pass[0]->GetDepth(), true);

	// Post processing passes 

	Material* LUT_TEST = new Material("LUTs/LUT_TEST.png");

	lut_cool = new LUT("LUTs/Bluedabadee.cube");
	lut_hot = new LUT("LUTs/ThatsHotBBY.cube");
	lut_custom = new LUT("LUTs/EdgeLord.cube");
	//post_pass.push_back(new LutColorCorrection(new LUT("LUTs/jungle.cube"), LUT_TEST->DIFF));
	//post_pass.push_back(new LutColorCorrection(lut_custom, main_pass->GetOutput()));
	//Textcontroller();

	highlightshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Highlights.frag");

	vergausshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/GausBlur.frag");
	vergausshader->SetB("isHorizontal", false);
	horgausshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/GausBlur.frag");
	horgausshader->SetB("isHorizontal", true);

	bloomshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Bloom.frag");
	pixelshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Pixelation.frag");


	//post_pass.push_back(new LutColorCorrection(new LUT("LUTs/jungle.cube"), LUT_TEST->DIFF));

	// Sampler2d INPUT
	post_pass.push_back(new PostProcess({ main_pass[0]->GetOutput(), light_buff[0]->GetOutput(), particle_buff[0]->GetOutput() }, new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Merge.frag")));

	Joint::init();
	//{91f62782-35bd-42df-85a1-8f359308dd0c}

	//// Init AudioEngine (Don't forget to shut down and update)
	//audioEngine.Init();

	//// Load a bank (Use the flag FMOD_STUDIO_LOAD_BANK_NORMAL)
	//audioEngine.LoadBank("Master", FMOD_STUDIO_LOAD_BANK_NORMAL);
	//
	////// Load an event
	//audioEngine.LoadEvent("MenuPlaceholder", "{91f62782-35bd-42df-85a1-8f359308dd0c}");
	//
	////// Play the event
	//audioEngine.PlayEvent("MenuPlaceholder");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Geo_pass.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Geo_pass.frag");
	skelDepth = new Shader("Shaders/depth_skel.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	lightPass = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/light_pass.frag");

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
	Material* playerMat = new Material("TreeTexture.png");

	Material* arenaTex = new Material("wood_texture.png");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "bone_t.bvh");
	Skeleton* gladiatorSkel2 = new Skeleton("Gladiator_Rig2", "gladiator2.bvh");


	SkelMesh* GladiatorMesh = new SkelMesh("gladiator.obj", gladiatorSkel, "WeightMap.png");
	SkelMesh* GladiatorMesh2 = new SkelMesh("Base_Gladiator.obj", gladiatorSkel2, "PlayerWeightMap.png");

	GladiatorMesh->SetAnim(gladiatorSkel->GetAnimByName("breathe"), 3, 0.34f, 0.5f);

	SkelMesh* P1_mesh = new SkelMesh(*GladiatorMesh2);
	SkelMesh* P2_mesh = new SkelMesh(*GladiatorMesh2);

	gladiatorSkel->WriteTree();
	gladiatorSkel2->WriteTree();
	//gladiatorSkel->Find("l_arm2")->animations[0][0].rotation += glm::vec3(0.0f, -90.0f, 0.0f);
	//gladiatorSkel->Find("l_arm1")->animations[0][0].scale = glm::vec3(2.0f, 2.0f, 2.0f);

	//wiggleSkel->Find("bone2")->animations[0][0].scale = glm::vec3(2.0f, 1.0f, 2.0f);

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	lights.push_back(new PointLight({ 4.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }, 0.2f, 0.7f, 0.9f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.3f, 0.7f, 0.2f, 0.07f, 0.017f));
	//lights.push_back(new PointLight({ 4.0, 4.0, 4.0 }, {1.0f, 0.0f, 0.0f}, 0.1, 0.5, 0.8, 0.7, 1.8));
	Load_Lights_From_File("");

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");
	Mesh* arena = new Mesh("ColitreeumV2.obj");
	Mesh* sword_mesh = new Mesh("Weapons/Sword.obj");
	Mesh* shield_mesh = new Mesh("Weapons/Circle_Shield.obj");


	Hitbox* basicCubeHB = new CubeHitbox(1.2f, 3.0f, 1.2f);
	Hitbox* basicCubeHB2 = new CubeHitbox(1.0, 1.0f, 1.0f);

	//Capsule testing
	Hitbox* basicCapsuleHB = new CapsuleHitbox(0.4f, 4.0f, entity); //radius + height
	Hitbox* basicCapsuleHB2 = new CapsuleHitbox(0.8f, 4.0f, entity);
	Hitbox* basicCapsuleHB3 = new CapsuleHitbox(0.2, 4.0);
	Hitbox* swordCapsuleHB = new CapsuleHitbox(0.09f, 12.8f);
	//Capsule Testing

	Hitbox* basicSphereHB = new SphereHitbox(1.0f);
	Hitbox* shieldSphereHB = new SphereHitbox(1.0f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);

	players.push_back(new Player(P1_mesh, playerMat, basicCapsuleHB, { 4.0f, 0.0f, 0.0f })); // P1
	players.push_back(new Player(P2_mesh, D20Tex, basicCapsuleHB2)); //P2

	GladiatorMesh2->SetAnim(GladiatorMesh2->GetSkeleton()->GetAnimByName("walk"), 0);
	GladiatorMesh2->SetFrame(0, 0);
	P1_mesh->SetIntensity(1, 0.5f);
	//players[PLAYER_1]->Rotate(glm::vec3(25, 0, 0));
	//shieldSphereHB->SetScale({0.2f, 1.0f, 0.1f});
	players[PLAYER_1]->dmgHP(50);

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ -6.0f, 0.0f, 0.0f });
	//players[PLAYER_2]->Rotate(glm::vec3(45,0,45));

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });


	test_player = new Player(GladiatorMesh2, defaultTex, basicCapsuleHB3, { 0.0f, 0.0f, 0.0f });
	//test_player->Scale(glm::vec3(1.2f));

	shieldSphereHB->SetScale(glm::vec3(0.1f, 0.65f, 0.65f));

	std::vector<std::string> OneHand_LC = { "sword_1", "sword_2" };

	Weapon* Hurt_Sword = new Weapon(sword_mesh, defaultTex, swordCapsuleHB, glm::vec3(-0.12f, -0.04f, -0.27f), OneHand_LC, 15.0f, 25.0f,0.20f, gladiatorSkel2->Find("r_hand"), P1_mesh);

	weapons.push_back(Hurt_Sword);
	shields.push_back(new Object(shield_mesh, defaultTex, shieldSphereHB, glm::vec3(0.0f, 0.0f, 0.0f), gladiatorSkel2->Find("l_hand"), P1_mesh));

	weapons[0]->Scale({ 0.9f, 0.9f, 0.9f });
	weapons[0]->SetRotation({ 0.0f, 90.0f, 90.0f });

	shields[0]->SetPosition({ 0.f, 0.f, 0.0f });
	shields[0]->Scale({ 0.8f, 0.8f, 0.8f });
	shields[0]->SetRotation({ 0.0f, 0.0f, 270.0f });

	//shields[0]->hitbox->SetPosition(glm::vec3(0.0f, 0.3f, 0.0f));
	//glm::vec3 test = shields[0]->hitbox->GetTransform().position;
	players[PLAYER_1]->SetWeapon(Hurt_Sword);
	players[PLAYER_1]->addChild(Hurt_Sword);
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
	Object* Colitreeum = new Object(arena, arenaTex, basicSphereHB, glm::vec3(0, -2, 0));

	Colitreeum->Scale(glm::vec3(2.5, 2.5, 2.5));

	floor->Move({ 0.0f, -0.75f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });



	//terrain.push_back(floor);
	terrain.push_back(Colitreeum);

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

	//DUUDE = new SplineMan(BsMesh, BsMat, basicCubeHB, glm::vec3(1, 1, 1), beacons);

	Cam = {
		new Camera({ -4.0f, 4.0f, 4.0f }, glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y))
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

	fire = new ParticleEngine({ 1.0f, 0.0f, 1.0f }, { 0.5f, 0.5f }, 4, 1.0f, { {new Material("redPlayer.png")} }, { ParticleEngine::FireEngineBehavior }, { Particle::FireUpdate });

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

	unsigned int tmp_cube;
	lightPass->Use();
	glGenTextures(1, &tmp_cube);
	glActiveTexture(GL_TEXTURE22);
	glBindTexture(GL_TEXTURE_CUBE_MAP, tmp_cube);
	for (unsigned int i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			10, 10, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	for (int z = 0; z < 28; z++)
		lightPass->SetI("lights[" + std::to_string(z) + "].depthMap", 22);
}

void OnePlayer::Load_Lights_From_File(std::string f)
{
	std::ifstream file("lights_list.li");

	while (!file.eof()) {
		std::string tmp;
		std::stringstream line;
		std::getline(file, tmp);

		line.str(tmp);

		std::string read;

		glm::vec3 pos, color;

		line >> read;
		pos.x = std::stof(read);
		line >> read;
		pos.y = std::stof(read); 
		line >> read;
		pos.z = std::stof(read);

		line >> read;
		color.x = std::stof(read);
		line >> read;
		color.y = std::stof(read);
		line >> read;
		color.z = std::stof(read);

		line >> read;
		float a = std::stof(read);
		line >> read;
		float d = std::stof(read);
		line >> read;
		float s = std::stof(read);

		line >> read;
		float l = std::stof(read);
		line >> read;
		float q = std::stof(read);

		lights.push_back(new PointLight(pos, color, a, d, s, l, q));
	}
}

void OnePlayer::ResizeCams()
{
	Scene::ResizeCams();
	main_pass[0]->Resize();
	light_buff[0]->Resize();
	particle_buff[0]->Resize();

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->buff->Resize();
	}
}


/*******************************************************************************************
*	Two Player Scene Functions
*******************************************************************************************/

TwoPlayer::TwoPlayer(WeaponType p1_weapon, ShieldType p1_shield, WeaponType p2_weapon, ShieldType p2_shield)
{
	_Weapons[PLAYER_1] = p1_weapon;
	_Weapons[PLAYER_2] = p2_weapon;
	_Shields[PLAYER_1] = p1_shield;
	_Shields[PLAYER_2] = p2_shield;

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

	static bool p_p = false;
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !p_p) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		p_p = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) {
		p_p = false;
	}

	static bool p_2 = false;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !p_2) {
		disable_lights = !disable_lights;
		p_2 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) {
		p_2 = false;
	}

	static bool p_3 = false;
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !p_3) {
		disable_tex = !disable_tex;
		p_3 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) {
		p_3 = false;
	}

	static bool p_lb = false;
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !p_lb) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}

		post_pass.push_back(new PostProcess({ post_pass.at(0)->buff->GetOutput() }, highlightshader));
		post_pass.push_back(new PostProcess({ post_pass.at(1)->buff->GetOutput() }, horgausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(2)->buff->GetOutput() }, vergausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(3)->buff->GetOutput(),post_pass.at(0)->buff->GetOutput() }, bloomshader));
		post_pass.push_back(new PostProcess({ post_pass.at(4)->buff->GetOutput() }, highlightshader));
		post_pass.push_back(new PostProcess({ post_pass.at(5)->buff->GetOutput() }, horgausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(6)->buff->GetOutput() }, vergausshader));
		post_pass.push_back(new PostProcess({ post_pass.at(7)->buff->GetOutput(),post_pass.at(0)->buff->GetOutput() }, bloomshader));

		p_lb = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) {
		p_lb = false;
	}

	static bool p_rb = false;
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !p_rb) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new PostProcess({ post_pass.at(0)->buff->GetOutput() }, pixelshader));
		p_rb = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) {
		p_rb = false;
	}

	static bool p_8 = false;
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && !p_8) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_hot, post_pass.at(0)->buff->GetOutput()));

		p_8 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) {
		p_8 = false;
	}

	static bool p_9 = false;
	if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !p_9) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_cool, post_pass.at(0)->buff->GetOutput()));

		p_9 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) {
		p_9 = false;
	}

	static bool p_0 = false;
	if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !p_0) {
		while (post_pass.size() > 1) {
			post_pass.pop_back();
		}
		post_pass.push_back(new LutColorCorrection(lut_custom, post_pass.at(0)->buff->GetOutput()));

		p_0 = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
		p_0 = false;
	}
}

void TwoPlayer::Update(float dt)
{

	for (int c = 0; c < players.size(); c++) {
		players[c]->Update(dt);

		for (int p = 0; p < players.size(); p++) {

			if (combo[c]) {
				comboTime[c] -= dt;
				if (comboTime[c] <= 0) {
					combo[c] = false;
					comboMult[c] = 1.0f;
				}
			}

			if (tauntTime[c] > 0)
				tauntTime[c] -= dt;
			else
				taunted[c] = 1.0f;

			if (players[c]->GetState() == taunting) {
				taunted[c] = 2.0f;
				tauntTime[c] = MAX_TAUNT;
			}

			if (players[c] != players[p]) {


				//Player Collision
				if (players[c]->HitDetect(players[p])) {
					std::cout << "Welp\n";
				}

				//problem -> weapon hitdetect is not seeing the shield

				//if (players[c]->GetState() == attacking && players[c]->GetWeapon()->HitDetect(players[p]->GetShield()) && !players[c]->GetWeapon()->getCooldown()) {
				//	if (players[p]->GetShield()->hitbox->GetType() == COLLISION_TYPE::shield) {
				//		std::cout << "Blocked!\n";
				//		audioEngine->PlayEvent("Block");
				//		players[p]->dmgHP(players[c]->GetWeapon()->GetDamage() - (players[p]->GetShield()->GetReduction() * players[c]->GetWeapon()->GetDamage()));
				//		players[p]->dmgSTAM(players[c]->GetWeapon()->GetDamage() * players[p]->GetShield()->GetStaminaCost());
				//	}
				//}

				

				//THIS IS THE ORIGINAL
				if (players[c]->GetFrameState() == FrameStates::Attack && players[c]->GetWeapon()->HitDetect(players[p]) && !players[c]->GetWeapon()->getCooldown()) {

					if (players[p]->hitbox->GetType() == entity) { //Making sure we hit the right hitbox
						std::cout << "Hit!\n";

						if (players[p]->GetFrameState() == FrameStates::Hold && players[p]->GetState() == blocking && players[p]->isInfront(players[p]->getFaceDir(), players[c]->GetPosition() - players[p]->GetPosition())) { //Are we blocking? Are we infront of the enemy?

							std::cout << "Blocked!\n";

							if (players[p]->GetShield() != nullptr) { //If he has a shield.

								audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
								audioEngine.LoadEvent("Block", "{1058aacd-0878-4ea8-be1c-e68f1e06cbc4}");
								audioEngine.PlayEvent("Block");
								players[p]->dmgHP(players[c]->GetWeapon()->GetDamage() - (players[p]->GetShield()->GetReduction() * players[c]->GetWeapon()->GetDamage()));
								players[p]->dmgSTAM(players[c]->GetWeapon()->GetDamage() * players[p]->GetShield()->GetStaminaCost());

								players[c]->GetWeapon()->setCooldown(true); 
							}
							else { // Use weapons stuff

								players[p]->dmgHP(players[c]->GetWeapon()->GetDamage() - ( players[p]->GetWeapon()->GetReduction() * players[c]->GetWeapon()->GetDamage()));
								players[p]->dmgSTAM(players[c]->GetWeapon()->GetDamage() * ((players[p]->GetWeapon()->GetStaminaCost() + 15.0f) * 0.01f) );

								players[c]->GetWeapon()->setCooldown(true);

							}
						}
						else //Clean hit.
						{ 
							static bool rollCool[2] = { false, false };

							if (players[p]->GetFrameState() != FrameStates::Roll) {
								players[p]->dmgHP(players[c]->GetWeapon()->GetDamage());
								players[p]->Hitstun();
								players[c]->GetWeapon()->setCooldown(true);
								audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
								audioEngine.LoadEvent("Hit", "{3830d309-eab3-4e2d-9cc2-2b00807daf5c}");
								audioEngine.PlayEvent("Hit"); 
								curScore += (20 * taunted[c] * comboMult[c]);
								combo[c] = true;
								comboTime[c] = MAX_COMBO;
								comboMult[c] += 0.5f;
							}
							else if(players[p]->GetState() == rolling){
								if (!rollCool[p]) {
									curScore += (5 * taunted[p] * comboMult[p]);
									combo[p] = true;
									comboTime[p] = MAX_COMBO;
									comboMult[p] += 0.5f;
									rollCool[p] = true;
								}
							}
							else {
								rollCool[p] = false;
							}
						}
					}
				
				
				}


				//if (players[c]->GetFrameState() == FrameStates::Attack){
				//	if (!players[c]->GetWeapon()->getCooldown()) {
				//		if (players[c]->GetWeapon()->HitDetect(players[p])) {
				//			if (players[p]->hitbox->GetType() == entity) {
				//				std::cout << "Hit!\n";
				//				audioEngine->PlayEvent("Hit");
				//				players[p]->dmgHP(players[c]->GetWeapon()->GetDamage());
				//				players[c]->GetWeapon()->setCooldown(true);
				//			}
				//		}
				//	}
				//}

				
			}
		}

		if (glfwJoystickPresent(c) && glfwJoystickIsGamepad(c)) {
			Cam[c]->Move(players[c]->phys.move, dt);

			players[c]->ApplyMove();

			Cam[c]->SetTarget(players[c]->GetPosition() + glm::vec3(0.0f, 1.5f, 0.0f));
		}
	}


	audioEngine.Update();

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

	for (int p = 0; p < particle_engines.size(); p++) {
		particle_engines[p].Update(dt);
	}
	
	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));

	((MorphMesh*)(morphyBoi->GetMesh()))->Update(dt);

	if (players[0]->GetHP() <= 0 || players[1]->GetHP() <= 0)
	{
		if (players[0]->GetHP() <= 0 && winannounce == false)
		{
			P2wins++;
			winannounce = true;
			audioEngine.PlayEvent("P2 Wins");
			
		}
		if (players[1]->GetHP() <= 0 && winannounce == false)
		{
			P1wins++;
			winannounce = true;
			audioEngine.PlayEvent("P1 Wins");
		}
		deathtimer -= dt;
		if (deathtimer <= 0)
		{
			RoundCount++;
			if (P2wins >= 3 || P1wins >= 3)
			{
				audioEngine.PlayEvent("Glory");
				RoundCount = 1;
				P1wins = 0;
				P2wins = 0;
				Game::CURRENT->setScene(SCENES::CHARACTER_SCENE);
			}
			scoreSub = 1.0f;
			curScore = MAX_SCORE;
			//setScene(MainMenu);
			Reset();
		}
	}

	// Crowd Stuff
	subIncreaser -= dt;
	if (subIncreaser <= 0) {
		subIncreaser = INCREASE_TIME;
		scoreSub++;
	}

	if (curScore <= 0)
	{
		players[PLAYER_1]->dmgHP(1 * dt);
		players[PLAYER_2]->dmgHP(1 * dt);
	}

	if (curScore > MAX_SCORE)
		curScore = MAX_SCORE;

	if (curScore > 0) 
	curScore -= dt * scoreSub;

	if (CrowdBoi != nullptr) {
		CrowdBoi->setScore(curScore);
	}
}
 
//int PlayScene::P2wins = 0;
//int PlayScene::P1wins = 0;
//int PlayScene::RoundCount = 1;

void TwoPlayer::Draw()
{
	ClearBuffs();

	glCullFace(GL_FRONT);
	for (int l = 0; l < lights.size(); l++) {
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights[l]->GetFrameBuffer());
		glClear(GL_DEPTH_BUFFER_BIT);
		lights[l]->SetupDepthShader(depthShader);
		lights[l]->SetupDepthShader(skelDepth);
		RenderScene(depthShader, skelDepth);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		glActiveTexture(GL_TEXTURE4 + l);
		glBindTexture(GL_TEXTURE_CUBE_MAP, lights[l]->GetDepthMap());
	}
	glCullFace(GL_BACK);

	for (int c = 0; c < lights.size(); c++) {
		lights[c]->SetupLight(shaderObj, c);
		lights[c]->SetupLight(morphShader, c);
		lights[c]->SetupLight(skelShader, c);
		lights[c]->SetupLight(lightPass, c);
	}
	shaderObj->SetI("num_lights", lights.size());
	morphShader->SetI("num_lights", lights.size());
	skelShader->SetI("num_lights", lights.size());
	lightPass->SetI("num_lights", lights.size());

	for (int c = 0; c < Cam.size(); c++) {
		main_pass[c]->Use();

		Cam[c]->SetupCam(shaderObj);
		Cam[c]->SetupCam(skelShader);

		shaderObj->SetB("CG_DISABLE_TEX", disable_tex);
		skelShader->SetB("CG_DISABLE_TEX", disable_tex);

		Sky->Draw(Cam[c]);
		RenderScene(shaderObj, skelShader);
		Cam[c]->SetupCam(morphShader);
		//morphyBoi->Draw(morphShader, Cam);

		particle_buff[c]->Use();

		for (int p = 0; p < particle_engines.size(); p++) {
			particle_engines[p].Draw(Cam[c]);
		}

		//glDisable(GL_DEPTH_TEST);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//
		//players[0]->hitbox->Draw(shaderObj, players[PLAYER_1]->GetTransform().GetWorldTransform());
		//players[1]->hitbox->Draw(shaderObj, players[PLAYER_2]->GetTransform().GetWorldTransform());
		//
		//players[0]->GetWeapon()->hitbox->Draw(shaderObj, players[0]->GetWeapon()->getParentTransform());
		//players[1]->GetWeapon()->hitbox->Draw(shaderObj, players[1]->GetWeapon()->getParentTransform());
		//
		//players[0]->GetShield()->hitbox->Draw(shaderObj, players[0]->GetShield()->getParentTransform());
		//players[1]->GetShield()->hitbox->Draw(shaderObj, players[1]->GetShield()->getParentTransform());
		//
		//
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glEnable(GL_DEPTH_TEST);
		if (!disable_lights) {
			Cam[c]->SetupPostLight(lightPass, 0);

			lightPass->SetI("num_cams", 1);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			glBindTexture(GL_TEXTURE_2D, main_pass[c]->GetOutput(1));
			lightPass->SetI("normals", 0);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, main_pass[c]->GetDepth());
			lightPass->SetI("depth", 1);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, main_pass[c]->GetOutput(2));
			lightPass->SetI("spec", 2);

			light_buff[c]->Use();
			lightPass->Use();

			Game::QUAD->Draw(lightPass);
		}
	}

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, post_pass.at(post_pass.size() - 1)->buff->GetOutput());
	POST_OUT->SetI("INPUT", 0);

	Game::QUAD->Draw(POST_OUT);

	glViewport(0, 0, Game::SCREEN.x, Game::SCREEN.y);

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}

	if (players[0]->GetHP() <= 0 && players[1]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Draw", SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, 1.0f, glm::vec3(1.f, 0.f, 0.f));
		//deathtimer -= 
	}
	else if (players[0]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Player 2 Wins", SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, 1.0, glm::vec3(1.f, 0.f, 0.f));

	}
	else if (players[1]->GetHP() <= 0)
	{
		Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Player 1 Wins", SCREEN_HEIGHT / 2.f, SCREEN_WIDTH / 2.f, 1.0, glm::vec3(1.f, 0.f, 0.f));

	}
	//std::string output1 = Name1[0] + " " + Name1[1] + " " + Name1[2];
	//std::string output2 = Name2[0] + " " + Name2[1] + " " + Name2[2];

	// + " Wins: " + std::to_string(P1wins);
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name1[0] + " " + Name1[1] + " " + Name1[2], 10, 580, 0.40, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Wins: " + std::to_string(P1wins), 50, 490, 0.60, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, Name2[0] + " " + Name2[1] + " " + Name2[2], 470, 580, 0.40, glm::vec3(1.f, 1.f, 1.f));
	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Wins: " + std::to_string(P2wins), 670, 490, 0.60, glm::vec3(1.f, 1.f, 1.f));

	Textcontroller->RenderText(TextRenderer::TEXTSHADER, "Round: " + std::to_string(RoundCount), 360, 450, 0.60, glm::vec3(1.f, 1.f, 1.f));

	glEnable(GL_DEPTH_TEST);
}

void TwoPlayer::LoadScene()
{
	Cam = {
		new Camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y)), // Cam 1
		new Camera(glm::vec3(2.0f, 0.0f, -4.0f), glm::vec4(0,0, Game::SCREEN.x, Game::SCREEN.y)) // Cam 2
	};

	for (int c = 0; c < Cam.size(); c++) {
		main_pass[c] = new FrameBuffer();
		main_pass[c]->AddComponent();
		main_pass[c]->AddComponent();
		light_buff[c] = new FrameBuffer();
		particle_buff[c] = new FrameBuffer(main_pass[c]->GetDepth(), true);
	}

	post_pass.push_back(new PostProcess({ main_pass[0]->GetOutput(), light_buff[0]->GetOutput(), particle_buff[0]->GetOutput(), main_pass[1]->GetOutput(), light_buff[1]->GetOutput(), particle_buff[1]->GetOutput() }, new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Merge2.frag")));

	//audioEngine.Init(); 

	audioEngine.Init();



	switch (RoundCount)
	{
	case 1:
		audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		audioEngine.LoadEvent("Round1", "{287449cd-0f8c-4e01-b817-f7de974921f7}");
		audioEngine.PlayEvent("Round1");

		//audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		//audioEngine.LoadEvent("Engine_Start", "{329a5270-2532-46b0-9b28-7f1f2b627efa}");
		//audioEngine.LoadEvent("Engine_Running_2", "{25d4404c-7bfe-49cc-a5d6-4c558667375d}");
		//audioEngine.LoadEvent("GlassBreak", "{0cda6a63-08d1-49f6-9d08-aa9890357558}");
		//
		//audioEngine.PlayEvent("Engine_Running_2");
		break;
	case 2:
		audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		audioEngine.LoadEvent("Round2", "{a95cb409-79f2-4131-9f69-7530899d00fd}");
		audioEngine.PlayEvent("Round2");
		break;
	case 3:
		audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		audioEngine.LoadEvent("Round3", "{8ac23920-72c4-4867-8ae6-c36fb8de6214}");
		audioEngine.PlayEvent("Round3");
		break;
	case 4:
		audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		audioEngine.LoadEvent("Round4", "{1b41bcb5-c7e2-4032-9c7f-ea1f7fe37002}");
		audioEngine.PlayEvent("Round4");
		break;
	case 5:
		audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
		audioEngine.LoadEvent("FinalRound", "{c692a986-0db6-40d6-8b67-b9a049bdc6a3}");
		audioEngine.PlayEvent("FinalRound");
		break;

	}
	audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
	audioEngine.LoadEvent("BiggerBiggerCrowed", "{a36c2fde-3e9f-4557-8e4c-8a516095b57e}");
	audioEngine.PlayEvent("BiggerBiggerCrowed");
	//audioEngine.LoadEvent("BattleMusic", "{5d5be828-f39b-4d9e-9bf2-ea581daa0e20}");
	//audioEngine.PlayEvent("BattleMusic"); 

	//audioEngine.LoadBank("CarCrash", FMOD_STUDIO_LOAD_BANK_NORMAL);
	//audioEngine.LoadEvent("Engine_Start", "{329a5270-2532-46b0-9b28-7f1f2b627efa}");
	//audioEngine.LoadEvent("Engine_Running_2", "{25d4404c-7bfe-49cc-a5d6-4c558667375d}");
	//audioEngine.LoadEvent("GlassBreak", "{0cda6a63-08d1-49f6-9d08-aa9890357558}");
	//
	//audioEngine.PlayEvent("Engine_Running_2");
	morphShader = new Shader("Shaders/Basic_Morph - NM.vert", "Shaders/Basic_Shader - NM.frag");

	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Geo_pass.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Geo_pass.frag");
	skelDepth = new Shader("Shaders/depth_skel.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	lightPass = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/light_pass.frag");

	//Post Process Shaders
	highlightshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Highlights.frag");
	vergausshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/GausBlur.frag");
	vergausshader->SetB("isHorizontal", false);
	horgausshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/GausBlur.frag");
	horgausshader->SetB("isHorizontal", true);
	bloomshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Bloom.frag");
	pixelshader = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/Pixelation.frag");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-normal.png");
	Material* playerMat1 = new Material("TreeTextureRED.png");
	Material* playerMat2 = new Material("TreeTextureBLU.png");

	Material* arenaTex = new Material("CaulitreeumTexture.png");
	Material* floorTex = new Material("sand.png", "sandnormal.png");

	Material* mainUI = new Material("hpbarcool.png");
	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("crowdbar.png");
	Material* blackBarMat = new Material("black.png");

	lut_cool = new LUT("LUTs/Bluedabadee.cube");
	lut_hot = new LUT("LUTs/ThatsHotBBY.cube");
	lut_custom = new LUT("LUTs/EdgeLord.cube");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 155.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.4f, 0.7f, 0.9f);
	lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 1.0f, 1.0f },  0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
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

	Sky = new Skybox("skybox/front.png", "skybox/right.png", "skybox/back.png", "skybox/left.png", "skybox/top.png", "skybox/bottom.png");


	DUUDE = new SplineMan(BsMesh, BsMat, basicCubeHB, glm::vec3(1, 1, 1), beacons);

	//sun = new DirectionalLight(glm::normalize(glm::vec3(1.5f, 1.0f, 0.5f)), { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f);
	//lights.push_back(new PointLight({ 0.0f, 30.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.014f, 0.0007f));
	//lights.push_back(new PointLight({ -4.0f, 1.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj"); 
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");

	Mesh* arena = new Mesh("Caulitreeum.obj");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "gladiator2.bvh");
	SkelMesh* P1_MESH = new SkelMesh("Base_Gladiator.obj", gladiatorSkel, "PlayerWeightMap.png");
	SkelMesh* P2_MESH = new SkelMesh(*P1_MESH);

	gladiatorSkel->WriteTree();

	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);

	//Capsule testing ********* PLAYER HITBOXES

	Hitbox* basicCapsuleHB = new CapsuleHitbox(0.4f, 2.3f, entity); //radius + height
	Hitbox* basicCapsuleHB2 = new CapsuleHitbox(0.4f, 2.3f, entity);

	players.push_back(new Player(P1_MESH, playerMat1, basicCapsuleHB, { 0.0f, -0.6f, 0.0f })); // THIS IS PLAYER ONE
	players[PLAYER_1]->hitbox->parentTransform(players[PLAYER_1]->GetTransform());

	players[PLAYER_1]->Rotate(glm::vec3(0, 0, 0));

	//players[PLAYER_1]->Rotate(glm::vec3(0.0f,90.0f,0.0f));

	players.push_back(new Player(P2_MESH, playerMat2, basicCapsuleHB2)); // THIS IS PLAYER TWO
	players[PLAYER_2]->hitbox->parentTransform(players[PLAYER_2]->GetTransform());

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, -0.6f, 0.0f });

	//////////////////////////////////Audio
	
	


	//audioEngine.LoadEvent("P1 Wins", "{1251a18c-193c-4301-8578-9d9329038cb4}");
	//audioEngine.PlayEvent("P2 Wins");
	//audioEngine.SetEventPosition("P1 Wins", players[1]->GetPosition());

	/////////////////////////////////////

	//terrain.push_back(new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	//terrain.b(glm::vec3(1.2f));

	Object* BOI = new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f });
	//floor->Move({ 0.0f, -1.5f, 0.0f });
	BOI->Scale(glm::vec3(1.2f));
	//terrain.push_back(BOI);
	//players[PLAYER_2]->Scale(glm::vec3(0.75f, 0.75f, 0.75f));

	Object* die = new Object(Square, DiceTex, basicCubeHB);
	die->Move({ 8.0f, 1.0f, 19.0f });
	//terrain.push_back(die);

	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			Object* Arenafloor = new Object(Game::QUAD, floorTex, basicCubeHB);
			Arenafloor->Move({ -80.0f + (20.0f * x), -0.65, -80.0f + (20.0f * y) });
			Arenafloor->Scale({ 10.0, 10.0f, 10.0f });
			Arenafloor->Rotate({ -90,0,0 });

			terrain.push_back(Arenafloor);
		}
	}

	Object* Colitreeum = new Object(arena, arenaTex, basicSphereHB, glm::vec3(0.f, 0.f, 0.f));

	Colitreeum->Scale(glm::vec3(0.65f, 0.65f, 0.65f));

	terrain.push_back(Colitreeum);


	Mesh* SwordMesh = nullptr;
	Mesh* SpearMesh = nullptr;
	Mesh* HammerMesh = nullptr;
	Mesh* ShieldMesh = nullptr;
	Mesh* BucklerMesh = nullptr;

	Material* SwordMat = nullptr;
	Material* SpearMat = nullptr;
	Material* HammerMat = nullptr;
	Material* ShieldMat = nullptr;
	Material* BucklerMat = nullptr;

	Hitbox* swordCapsuleHB = new CapsuleHitbox(0.15f, 1.f);
	swordCapsuleHB->SetScale(glm::vec3(1.0));

	Hitbox* shieldSphereHB = new SphereHitbox(1.0f, COLLISION_TYPE::shield);
	shieldSphereHB->SetScale(glm::vec3(0.1f, 0.65f, 0.65f));

	std::vector<std::string> OneHand_LC = {"sword_1", "sword_2", "sword_3"};

	//Object* P1_sword = new Object(SwordMesh, defaultTex, swordCapsuleHB, { -0.12f, -0.04f, -0.27f }, gladiatorSkel->Find("r_hand"), P1_MESH);
	//Object* P2_sword = new Object(SwordMesh, defaultTex, swordCapsuleHB, { -0.12f, -0.04f, -0.27f }, gladiatorSkel->Find("r_hand"), P2_MESH);

	for (int p = 0; p <= PLAYER_2; p++) {
		Weapon* weapon;
		Shield* shield;

		SkelMesh* mesh;

		if (p == PLAYER_1) {
			mesh = P1_MESH;
		}
		else {
			mesh = P2_MESH;
		}
		
		switch (_Weapons[p]) {
		case WEAPON_HAMMER:
			if (HammerMesh == nullptr) {
				HammerMesh = new Mesh("Weapons/Warhammer_Maul.obj");
			}

			if (HammerMat == nullptr) {
				HammerMat = new Material("Weapons/tex/warhammer.png");
			}
			weapon = new Weapon(HammerMesh, HammerMat, swordCapsuleHB, glm::vec3(-0.12f, 0.025f, -0.5f), OneHand_LC, "", "", 15.0f, 25.0f, 0.20f, gladiatorSkel->Find("r_hand"), mesh);
			weapon->SetRotation({90.0f, 90.0f, 0.0f});
			weapon->Scale({1.2f, 1.2f, 1.2f});
			
			break;
		// Spear
		case WEAPON_SPEAR:
			if (SpearMesh == nullptr) {
				SpearMesh = new Mesh("Weapons/Spear.obj");
			}

			if (SpearMat == nullptr) {
				SpearMat = new Material("Weapons/tex/spear.png");
			}
			weapon = new Weapon(SpearMesh, SpearMat, swordCapsuleHB, glm::vec3(-0.12f, 0.025f, -0.5f), OneHand_LC, "", "", 15.0f, 25.0f, 0.20f, gladiatorSkel->Find("r_hand"), mesh);
			weapon->SetRotation({ 90.0f, 90.0f, 0.0f });

			break;

		// Sword
		case WEAPON_SWORD:
		default:
			if (SwordMesh == nullptr) {
				SwordMesh = new Mesh("Weapons/Sword.obj");
			}

			if (SwordMat == nullptr) {
				SwordMat = new Material("Weapons/tex/sword.png");
			}

			weapon = new Weapon(SwordMesh, SwordMat, swordCapsuleHB, glm::vec3(-0.12f, 0.025f, -0.15f), OneHand_LC, "", "", 15.0f, 25.0f,0.20f, gladiatorSkel->Find("r_hand"), mesh);
			weapon->Scale({ 0.9f, 0.9f, 0.9f });

			weapon->SetRotation({ 0.0f, 90.0f, 90.0f });
			
			break;
		}

		switch (_Shields[p]) {
		case SHIELD_NONE:

			break;
		case SHIELD_BUCKLER:
			if (BucklerMesh == nullptr) {
				BucklerMesh = new Mesh("Weapons/Buckler.obj");
			}

			if (BucklerMat == nullptr) {
				BucklerMat = new Material("Weapons/tex/buckler.png");
			}

			shield = new Shield(BucklerMesh, BucklerMat, shieldSphereHB, { -0.30f, 0.1f, 0.05f }, 0.6f, 0.25f, gladiatorSkel->Find("l_hand"), mesh);
			shield->SetRotation({ 0.0f, 0.0f, 270.0f });
			shield->Scale({0.85f, 0.85f, 0.85f});
			break;

		case SHIELD_LARGE:
		default:
			if (ShieldMesh == nullptr) {
				ShieldMesh = new Mesh("Weapons/CircleShield.obj");
			}

			if (ShieldMat == nullptr) {
				ShieldMat = new Material("Weapons/tex/shield.png");
			}

			shield = new Shield(ShieldMesh, ShieldMat, shieldSphereHB, { -0.25f, 0.1f, 0.05f }, 0.6f, 0.25f, gladiatorSkel->Find("l_arm2"), mesh);
			shield->Scale(glm::vec3(0.85f));
			shield->SetRotation({ 0.0f, 0.0f, 265.0f });
			break;
		}


		players[p]->SetWeapon(weapon);
		players[p]->addChild(weapon);
		if (shield != nullptr) {
			players[p]->SetShield(shield);
			players[p]->addChild(shield);
		}
	}

	UI* hpBG = new UI(260, 10, { 55.0f, 544.5f, -1.0f }, blackBarMat);
	UI* stamBG = new UI(260, 10, { 55.0f, 524.50f, -1.0f }, blackBarMat);
	UI* crowdBG = new UI(80, 71, { 360.0f, 504.0f, -1.0f }, blackBarMat);

	UI* hpBG2 = new UI(260, 10, { 487.5f, 544.5f, -1.0f }, blackBarMat);
	UI* stamBG2 = new UI(260, 10, { 487.5f, 524.5f, -1.0f }, blackBarMat);
	CrowdBoi = new CrowdBar((Player*)players[PLAYER_1], glm::vec2(361, 505), crowdBarMat, crowdBG);

	ui = {
		new UI(801.5, 100, glm::vec3(0.0f, 490.0f, 0.0f), mainUI),
		new HealthBar((Player*)players[PLAYER_1], glm::vec2(60, 547), hpBarMat, hpBG),
		new StaminaBar((Player*)players[PLAYER_1], glm::vec2(60, 527), stamBarMat, stamBG),
		CrowdBoi,

		new HealthBar((Player*)players[PLAYER_2], glm::vec2(492.5, 547.5), hpBarMat, hpBG2),
		new StaminaBar((Player*)players[PLAYER_2], glm::vec2(492.5, 527.5), stamBarMat, stamBG2)
	};

	Material* toddMed = new Material("particles/toddMED.png");
	Material* ladyMed = new Material("particles/ladyMED.png");
	Material* chadMed = new Material("particles/chadMED.png");
	Material* nickMed = new Material("particles/nickMED.png");
	Material* stacyMed = new Material("particles/stacyMED.png");
	Material* kyleMed = new Material("particles/kyleMED.png");

	std::vector<std::vector<Material*>> crowd_mats = {
		{
		new Material("particles/toddLOW.png"),
		toddMed,
		new Material("particles/toddHIGH.png"),
		toddMed
		},
		{
		new Material("particles/ladyLOW.png"),
		ladyMed,
		new Material("particles/ladyHIGH.png"),
		ladyMed
		},
		{
		new Material("particles/chadLOW.png"),
		chadMed,
		new Material("particles/chadHIGH.png"),
		chadMed
		},
		{
		new Material("particles/nickLOW.png"),
		nickMed,
		new Material("particles/nickHIGH.png"),
		nickMed
		},
		{
		new Material("particles/stacyLOW.png"),
		stacyMed,
		new Material("particles/stacyHIGH.png"),
		stacyMed
		},
		{
		new Material("particles/kyleLOW.png"),
		kyleMed,
		new Material("particles/kyleHIGH.png"),
		kyleMed
		}
	};

	Material* rock_part = new Material("particles/rock.png");

	particle_engines = {
		ParticleEngine(glm::vec3(), glm::vec2(1.0f, 1.65f), ParticleEngine::MAX_PARTICLES, 0.15f, crowd_mats, {ParticleEngine::AudienceEngineBehavior}, {Particle::AudienceUpdate}),
		ParticleEngine(glm::vec3(), glm::vec2(0.5f, 0.5f), 40, 5.0f, {{rock_part}}, { ParticleEngine::RockEngine }, {Particle::RockUpdate})
	};

	std::vector<std::string> frames = { "wobble/wobble1.obj", "wobble/wobble2.obj", "wobble/wobble3.obj", "wobble/wobble4.obj" };
	morphyBoi = new Object(new MorphMesh(frames), defaultTex, basicCubeHB, glm::vec3(15.0f, 1.0f, 2.0f));

	Reset();
}

void TwoPlayer::Reset() {
	for (int p = 0; p < players.size(); p++) {
		players[p]->SetPosition({0.0f, -0.6, -20.0f + (40.0f * p) });
		//players[p]->SetPosition({ 0.0f, -0.6, -2.0f + (1.8f * p) });

		players[p]->SetRotation({0.0f, 180.0f * p, 0.0f});

		players[p]->Reset();
	}

	deathtimer = DEATH_TIME;

	winannounce = false;
}

void TwoPlayer::ResizeCams()
{
	Scene::ResizeCams();
	for (int c = 0; c < Cam.size(); c++) {
		main_pass[c]->Resize();
		light_buff[c]->Resize();
		particle_buff[c]->Resize();
	}

	for (int c = 0; c < post_pass.size(); c++) {
		post_pass[c]->buff->Resize();
	}
}