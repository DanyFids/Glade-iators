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

enum FrameStates {
	Neutral,
	Block,
	Roll,
	Attack,
	Deflect
};

ChannelType StringToChnlTp(std::string name);

class Skeleton;
class Shader;
class Mesh;
class Material;

struct Transform;

class Joint {
protected:
	static Mesh* node_mesh;
	static Material* node_mat;

	std::string name;
	Joint* parent;
	std::vector<Joint*> children;
	std::vector<ChannelType> channels;

	glm::vec3 offset;

public:
	static void init();

	std::vector<std::vector<Transform>> animations;

	Joint(std::string n, Joint* p);

	Joint* Find(std::string name);

	void WriteOutput(std::string pref);
	void WriteTransform(int anim, int frame);

	void Draw(glm::mat4 global, int a, int f, Shader* shdr);

	void FillJointArray(glm::mat4 * arr, glm::mat4 global, glm::vec3 * binds, glm::vec3 last, glm::vec3*& bind_t, glm::vec3 last_b, int& cur, int anim, int frame);
	void LoadAnimFrame(std::queue<float>&, int anim, int frame);

	glm::mat4 TransformTo(int anim, int frame);

	friend class Skeleton;
};

class Skeleton {
	std::string name;
	Joint* root;


	int num_bones;
public:
	std::vector<std::vector<FrameStates>> AnimStates;
	Skeleton(std::string name, std::string file);

	void WriteTree();

	int LoadFromFile(std::string file);
	Joint* Find(std::string name);
	void DrawSkeleton(glm::mat4 global, int a, int f, Shader* shdr) { root->Draw(global, a, f, shdr); }
	void GetTransformArray(glm::mat4* & bones, glm::vec3* & binds, glm::vec3* & bind_t, int anim, int frame);
	int GetNumBones() { return num_bones; }
	int GetNumFrames(int a) { return root->animations[a].size(); }
};