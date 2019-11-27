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

struct Transform;

class Joint {
protected:
	std::string name;
	Joint* parent;
	std::vector<Joint*> children;
	std::vector<ChannelType> channels;

	glm::vec3 offset;

public:
	std::vector<std::vector<Transform>> animations;

	Joint(std::string n, Joint* p);

	Joint* Find(std::string name);

	void WriteOutput(std::string pref);

	void FillJointArray(Transform* arr, int& cur, int anim, int frame);

	friend class Skeleton;
};

class Skeleton {
	std::string name;
	Joint* root;


	int num_bones;
public:
	Skeleton(std::string name, std::string file);

	void WriteTree();

	int LoadFromFile(std::string file);
	Joint* Find(std::string name);

	Transform* GetTransformArray(int anim, int frame);
	int GetNumBones() { return num_bones; }
	int GetNumFrames(int a) { return root->animations[a].size(); }
};