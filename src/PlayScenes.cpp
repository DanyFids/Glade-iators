#include "PlayScenes.h"
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<GLM/glm.hpp>
#include <iostream>
#include <typeinfo>
#include <random>


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

	for (int a = 0; a < attacks.size(); a++)
	{
		attacks[a]->time -= dt;
		if (attacks[a]->time <= 0)
		{
			attacks.erase(attacks.begin() + a);
			break;
		}
		if (attacks[a]->HitDetect(players[0]) && attacks[a]->player != 0)
		{
			std::cout << "Screaming\n";
		}
		if (attacks[a]->HitDetect(players[1]) && attacks[a]->player != 1)
		{
			std::cout << "Screaming\n";
		}
	}

	((MorphMesh*)(morphyBoi->GetMesh()))->Update(dt);
	// m morphyBoi->Update(dt);

	for (int a = 0; a < shields.size(); a++)
	{
		
		if (shields[a]->player == 0 && block1 == false)
		{
			shields.erase(shields.begin() + a);
			break;
		}
		
	}

	DUUDE->Update(dt);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));


	
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

		RenderScene(shaderObj);
		Cam[c]->SetupCam(morphShader);
		morphyBoi->Draw(morphShader, Cam);
		Cam[c]->SetupCam(skelShader);
		test_player->Draw(skelShader, Cam);
		
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
	skelShader = new Shader("Shaders/skeleton_shader.vert", "Shaders/Basic_Shader.frag");

	Material* DiceTex = new Material("dice-texture.png", "d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");
	
	Material* GladiatorWM = new Material("WeightMap.png");

	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");

	Material* arenaTex = new Material("wood_texture.png");

	Skeleton* gladiatorSkel = new Skeleton("Gladiator_Rig", "gladiator.bvh");
	SkelMesh* GladiatorMesh = new SkelMesh("gladiator.obj", gladiatorSkel, GladiatorWM);

	gladiatorSkel->WriteTree();
	gladiatorSkel->Find("l_arm1")->animations[0][0].position += glm::vec3(1.0f, 0.0f, 1.0f);
	gladiatorSkel->Find("l_arm2")->animations[0][0].position += glm::vec3(2.0f, 0.0f, 2.0f);
	gladiatorSkel->Find("r_arm1")->animations[0][0].position += glm::vec3(3.0f, 0.0f, 3.0f);
	gladiatorSkel->Find("r_arm2")->animations[0][0].position += glm::vec3(4.0f, 0.0f, 4.0f);
	gladiatorSkel->Find("r_leg1")->animations[0][0].position += glm::vec3(5.0f, 0.0f, 5.0f);




	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	//lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
	lights.push_back(new PointLight({ -4.0f, 4.0f, 4.0f }, { 1.0f, 1.0f, 1.0f }, 0.1f, 0.5f, 1.0f, 0.07f, 0.017f));

	Mesh* Square = new Mesh("d6.obj");
	Mesh* d20 = new Mesh("d20.obj");
	Mesh* boi = new Mesh("gladiator.obj");
	Mesh* arena = new Mesh("ColitreeumV2.obj");

	Hitbox* basicCubeHB = new CubeHitbox(1.0f,1.0f,1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);
	players.push_back(new Player(Square, DiceTex, basicCubeHB, { 3.0f, 0.3f, 0.0f }));
	players.push_back(new Player(d20, D20Tex, basicSphereHB));

	players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, 0.3f, 0.0f });
	

	test_player = new Player(GladiatorMesh, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f });
	test_player->Scale(glm::vec3(1.2f));
	players.push_back(new Player(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	players[2]->Scale(glm::vec3(1.2f)); 

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

	DUUDE = new SplineMan(BsMesh, BsMat, basicCubeHB, glm::vec3(1, 1, 1), beacons);

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

			Cam[c]->SetTarget(players[c]->GetPosition());
		}
	}

	for (int a = 0; a < attacks.size(); a++)
	{
		attacks[a]->time -= dt;
		if (attacks[a]->time <= 0)
		{
			attacks.erase(attacks.begin() + a);
			break;
		}
		if (attacks[a]->HitDetect(players[0]) && attacks[a]->player != 0)
		{
			std::cout << "Screaming\n";
		}
		if (attacks[a]->HitDetect(players[1]) && attacks[a]->player != 1)
		{
			std::cout << "Screaming\n";
		}
	}

	for (int a = 0; a < shields.size(); a++)
	{

		if (shields[a]->player == 0 && block1 == false)
		{
			shields.erase(shields.begin() + a);
			break;
		}
		if (shields[a]->player == 1 && block2 == false)
		{
			shields.erase(shields.begin() + a);
			break;
		}

	}

	DUUDE->Update(dt);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Update(dt);
	}

	shaderObj->SetVec3("indexColor", glm::vec3(0.0f, 1.0f, 0.0f));
}

void TwoPlayer::Draw()
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

	glDisable(GL_DEPTH_TEST);

	for (int u = 0; u < ui.size(); u++) {
		ui[u]->Draw(glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT));
	}
	glEnable(GL_DEPTH_TEST);

}

void TwoPlayer::LoadScene()
{
	shaderObj = new Shader("Shaders/Basic_Shader.vert", "Shaders/Basic_Shader.frag");
	depthShader = new Shader("Shaders/depth_shader.vert", "Shaders/depth_shader.frag", "Shaders/depthGeo.glsl");
	sunShader = new Shader("Shaders/sunDepth.vert", "Shaders/sunDepth.frag");

	Material* DiceTex = new Material("d6-normal.png");
	Material* D20Tex = new Material("d20-texture.png");
	//Material* SwordTex = new Material("sword-texture.png", "sword-norm.png");
	Material* defaultTex = new Material("default-texture.png", "default-texture.png");

	Material* hpBarMat = new Material("yuck.png");
	Material* stamBarMat = new Material("blue.png");
	Material* crowdBarMat = new Material("white.png");
	Material* blackBarMat = new Material("black.png");

	sun = new DirectionalLight(glm::normalize(glm::vec3(5.0f, 15.0f, 5.0f)), { 1.0f, 1.0f, 1.0f }, 0.2f, 0.5f, 0.8f);
	//lights.push_back(new PointLight({ 0.5f, 30.0f, 0.5f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, 0.3f, 0.5f, 1.0f, 0.014f, 0.0007f));
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

	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);
	Hitbox* basicSphereHB = new SphereHitbox(0.70f);
	Hitbox* BlockyBoiHB = new CubeHitbox(0.5f, 1.8f, 0.5f);

	players.push_back(new Player(Square, DiceTex, basicCubeHB, { 3.0f, 0.3f, 0.0f }));
	players.push_back(new Player(Square, DiceTex, basicCubeHB));

	//players[PLAYER_2]->Scale({ 0.75f,0.75f,0.75f });
	players[PLAYER_2]->Move({ 0.0f, 0.3f, 0.0f });

	//terrain.push_back(new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f }));
	//terrain.b(glm::vec3(1.2f));

	Object* BOI = new Object(boi, defaultTex, BlockyBoiHB, { -3.0f, 0.0f, 2.0f });
	//floor->Move({ 0.0f, -1.5f, 0.0f });
	BOI->Scale(glm::vec3(1.2f));
	terrain.push_back(BOI);
	//players[PLAYER_2]->Scale(glm::vec3(0.75f, 0.75f, 0.75f));

	Object* floor = new Object(Square, defaultTex, basicCubeHB);
	floor->Move({ 0.0f, -1.5f, 0.0f });
	floor->Scale({ 30.0f, 0.5f, 30.0f });

	terrain.push_back(floor);

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
}
