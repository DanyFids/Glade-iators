#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

Material::Material(std::string f, std::string n, std::string s)
{
	unsigned int texture[3];
	glGenTextures(3, texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(f.c_str(), &width, &height, &nrChannels, 0);

	if (data) {
		if(nrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << f  << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} 

	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int nwidth, nheight, nnrChannels;
	unsigned char* ndata = stbi_load(n.c_str(), &nwidth, &nheight, &nnrChannels, 0);

	if (ndata) {
		if (nnrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nwidth, nheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, ndata);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, nwidth, nheight, 0, GL_RGB, GL_UNSIGNED_BYTE, ndata);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE::NORM::ERROR_LOADING_FILE: '" << n << "' \n" << std::endl;
	}

	stbi_image_free(ndata);

	glBindTexture(GL_TEXTURE_2D, texture[2]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int swidth, sheight, snrChannels;
	unsigned char* sdata = stbi_load(s.c_str(), &swidth, &sheight, &snrChannels, 0);

	if (sdata) {
		if (snrChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, swidth, sheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, sdata);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, swidth, sheight, 0, GL_RGB, GL_UNSIGNED_BYTE, sdata);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "ERROR::TEXTURE::SPEC::ERROR_LOADING_FILE: '" << s << "'\n" << std::endl;
	}

	stbi_image_free(sdata);

	diff_file = f;
	norm_file = n;
	spec_file = s;
	DIFF = texture[0];
	NORM = texture[1];
	SPEC = texture[2];

	shine = 32;
}

Skybox::Skybox(std::string front, std::string right, std::string back, std::string left, std::string top, std::string bot)
{
	unsigned int texture;
	glGenTextures(1, &texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	stbi_set_flip_vertically_on_load(false);
	
	int width, height, nrChannels;
	unsigned char* data;
	
	//front
	data = stbi_load(front.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if(nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << front << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_Z);
	stbi_image_free(data);

	//right
	data = stbi_load(right.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << right << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X);
	stbi_image_free(data);

	//top
	data = stbi_load(top.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << top << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_Y);
	stbi_image_free(data);

	//back
	data = stbi_load(back.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << back << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z);
	stbi_image_free(data);

	//left
	data = stbi_load(left.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << left << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_NEGATIVE_X);
	stbi_image_free(data);

	//bot
	data = stbi_load(bot.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		if (nrChannels == 4)
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else {
		std::cout << "ERROR::TEXTURE::TEX::ERROR_LOADING_FILE: '" << bot << "'\n" << std::endl;
		unsigned char* miss_data = stbi_load("missing_tex.png", &width, &height, &nrChannels, 0);

		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, miss_data);
	}
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y);
	stbi_image_free(data);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	SKY = texture;
}

void Skybox::INIT()
{
}

Shader* Skybox::SHADER = nullptr;
Mesh* Skybox::CUBE = nullptr;

void Skybox::Draw(Camera* cam)
{
	if (SHADER == nullptr) {
		SHADER = new Shader("shaders/skybox.vert", "shaders/skybox.frag");
	}

	if (CUBE == nullptr) {
		CUBE = new Mesh("unit_cube.obj");
	}

	glDepthMask(GL_FALSE);
	glCullFace(GL_FRONT);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, SKY);
	SHADER->SetI("skybox", 0);

	SHADER->SetMat4("projection", cam->GetProject());
	SHADER->SetMat4("view", glm::mat4(glm::mat3( cam->GetLookAt())));

	SHADER->Use();

	CUBE->Draw(SHADER);

	glDepthMask(GL_TRUE);
	glCullFace(GL_BACK);
}


LUT::LUT(std::string f)
{
	std::ifstream file(f.c_str());

	if (!file) {
		std::cout << "ERROR::TEXTURE::LUT::ERROR_LOADING_FILE: '" << f << "'\n" << std::endl;
	}
	else {
		std::vector<RGB> data;

		while (!file.eof()) {
			std::string temp, read;
			std::stringstream line;

			std::getline(file, temp);

			if (temp.compare("") == 0 || temp[0] == '#') {
				continue;
			}
			
			line.str(temp);

			line >> read;

			if (read.compare("TITLE") == 0) {
				line >> read;
				name = read;
			}
			else if(read.compare("LUT_3D_SIZE") == 0){
				line >> read;
				LUT_SIZE = std::stoi(read);
			}
			else if (!isalpha(read[0])) {
				RGB t;
				t.r = std::stof(read);
				line >> read;
				t.g = std::stof(read);
				line >> read;
				t.b = std::stof(read);

				data.push_back(t);
			}
		}

		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_3D, ID);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		if (data.size() < LUT_SIZE * LUT_SIZE * LUT_SIZE) std::cout << "ERROR::TEXTURE::LUT::INCORECT_SIZE\n";

		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, LUT_SIZE, LUT_SIZE, LUT_SIZE, 0, GL_RGB, GL_FLOAT, data.data());
	}
}
