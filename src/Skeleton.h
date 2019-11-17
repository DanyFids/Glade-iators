#pragma once

#include<string>
#include<vector>
#include<GLM/glm.hpp>

enum ChannelType {
	INVALID_CHNL_TYPE,
	Xposition,
	Yposition,
	Zposition,
	Xrotation,
	Yrotation,
	Zrotation
};

ChannelType StringToChnlTp(std::string name);

class Skeleton;

class Joint {
protected:
	std::string name;
	Joint* parent;
	std::vector<Joint*> children;
	std::vector<ChannelType> channels;

	glm::vec3 offset;
	glm::vec3 rotation;
	glm::vec3 position;

public:
	Joint(std::string n, Joint* p);

	Joint* Find(std::string name);

	void WriteOutput(std::string pref);

	friend class Skeleton;
};

class Skeleton {
	std::string name;
	Joint* root;

public:
	Skeleton(std::string name, std::string file);

	void WriteTree();

	int LoadFromFile(std::string file);
	Joint* Find(std::string name);
};