#pragma once
#include<GLM/glm.hpp>
#include<vector>
#include <string>
class Material;
class Shader;
class Camera;
class Skeleton;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_uv;
	glm::vec3 tangent;
	glm::vec3 biTan;
	int id;
};

struct Pose {
	unsigned int id;

	unsigned int num_vert;
	unsigned int num_indi;
	std::vector<Vertex> vert_vec = std::vector<Vertex>();
	std::vector<unsigned int>indi_vec = std::vector<unsigned int>();

};

struct bigVert {
	glm::vec3 posi1;
	glm::vec3 norm1;
	glm::vec2 tex_uv;
	glm::vec3 tang1;
	glm::vec3 biTa1;
	int id1;
	glm::vec3 posi2;
	glm::vec3 norm2;
	glm::vec2 tex_2;
	glm::vec3 tang2;
	glm::vec3 biTa2;
	int id2;
};

class Mesh {
protected:
	unsigned int vao;
	unsigned int vbo;
	unsigned int buffers[2];
	int num_indices;
	Material * tex;
	glm::mat4 model;


public:
	Mesh();
	Mesh(float vert[], int num_vert, unsigned int indi[], int num_indi);
	Mesh(const char* file);
	~Mesh();

	virtual void Draw(Shader*);
	void SetTexture(Material* t);
	void Update(float dt);

	void LoadMesh(const char* f, std::vector<Vertex> &vertices, unsigned int& num_verts, std::vector<unsigned int> &indices, unsigned int& num_indi);

	void Move(glm::vec3 dir, float dt);
	void SetPosition(glm::vec3 pos);
};

class SkelMesh : public Mesh {
	int curFrame;
	int nexFrame;
	int anim = 0;

	Skeleton* skeleton;
	Material* weightMap;

public:
	SkelMesh(std::string f, Skeleton* s, Material* w);

	virtual void Draw(Shader*);
};

class MorphMesh : public Mesh {
	std::vector<std::string> keyFrames;
	std::vector<Pose> poses;

	int num_frames;
	int curFrame = 0;
	int nextFrame = 1;

	float time = 0.0f;

	const float ANIM_TIME = 2.0f;
	
	bool looping = false;
	bool reversed = false;
	bool paused = false;

public:
	MorphMesh(std::vector<std::string> keyframes);

	virtual void Draw(Shader*);
	void addPose( std::string filename);
	void Update(float dt);
	void NextPose();
	void PrevPose();
	
	void setLooping();
	void setReverse();
	void setPose(int frame, float time);
	void play();
	void pause();
};

