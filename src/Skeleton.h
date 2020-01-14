#pragma once

#include<string>
#include<vector>
#include<queue>
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
	void WriteTransform(int anim, int frame);

	void FillJointArray(glm::mat4 * arr, glm::mat4 global, glm::vec3 * binds, glm::vec3 last, glm::vec3*& bind_t, glm::vec3 last_b, int& cur, int anim, int frame);
	void LoadAnimFrame(std::queue<float>&, int anim, int frame);

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

	void GetTransformArray(glm::mat4* & bones, glm::vec3* & binds, glm::vec3* & bind_t, int anim, int frame);
	int GetNumBones() { return num_bones; }
	int GetNumFrames(int a) { return root->animations[a].size(); }
};