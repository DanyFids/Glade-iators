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

class Skybox {
public:
	unsigned int SKY;

	std::string sky_file;

	Skybox(std::string f);
};

class LUT {
public:
	std::string name;
	unsigned int LUT_SIZE;

	LUT(std::string file);

	unsigned int ID;

	struct RGB { float r, g, b; };
};