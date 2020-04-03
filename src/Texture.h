#pragma once
#include<string>

class Material {
public:
	unsigned int DIFF;
	unsigned int SPEC;
	unsigned int NORM;
	float shine;

	std::string diff_file;
	std::string spec_file;
	std::string norm_file;

	Material(std::string d, std::string n = "default-normal.png", std::string s = "default-texture.png");
};

class Camera;
class Shader;
class Mesh;

class Skybox {
public:
	static Shader* SHADER;
	static Mesh* CUBE;

	unsigned int SKY;

	std::vector<std::string> sky_file;

	Skybox(std::string front, std::string right, std::string back, std::string left, std::string top, std::string bot);

	static void INIT();

	void Draw(Camera* cam);
};

class LUT {
public:
	std::string name;
	unsigned int LUT_SIZE;

	LUT(std::string file);

	unsigned int ID;

	struct RGB { float r, g, b; };
};