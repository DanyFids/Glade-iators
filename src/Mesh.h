#pragma once
#include <GLM/glm.hpp>
#include <vector>
#include <string>
class Material;
class Shader;
class Camera;
class Skeleton;
enum FrameStates;

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
	std::vector<Vertex> vertices;

public:
	Mesh();
	Mesh(float vert[], int num_vert, unsigned int indi[], int num_indi);
	Mesh(const char* file);
	~Mesh();

	virtual void Draw(Shader*);
	void SetTexture(Material* t);
	virtual void Update(float dt);

	void LoadMesh(const char* f, std::vector<Vertex> &vertices, unsigned int& num_verts, std::vector<unsigned int> &indices, unsigned int& num_indi);

	void Move(glm::vec3 dir, float dt);
	void SetPosition(glm::vec3 pos);
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
	virtual void Update(float dt);
	void NextPose();
	void PrevPose();
	
	void setLooping();
	void setReverse();
	void setPose(int frame, float time);
	void play();
	void pause();
};

struct SkelVert {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_uv;
	glm::vec3 tangent;
	glm::vec3 biTan;
	int id;
	glm::ivec4 bone_ids;
	glm::vec4 weights;

	SkelVert(Vertex v) {
		position = v.position;
		normal = v.normal;
		tex_uv = v.tex_uv;
		tangent = v.tangent;
		biTan = v.biTan;
		id = v.id;
	}
};

class SkelMesh : public Mesh {
	static const int num_channels = 4;
	
	int curFrame[num_channels] = {0, 0, 0, 0};
	int nexFrame[num_channels] = {0,0,0,0};
	int anim[num_channels] = { 0, -1, -1, -1 };

	float play_spd[num_channels] = { 1.0f, 0.0f, 0.0f, 0.0f };
	float anim_time[num_channels] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float chnl_intensity[num_channels] = { 1.0f, 0.0f, 0.0f, 0.0f };

	Skeleton* skeleton;
	//Material* weightMap;

	//glm::vec3 root_pos, p_root_pos;

public:
	SkelMesh(std::string f, Skeleton* s, std::string weightMap);
	~SkelMesh();

	virtual void Update(float dt);

	virtual void Draw(Shader*);
	void SetAnim(int id, unsigned int chnl, float i = 1.0f, float s = 1.0f);
	void SetFrame(unsigned int id, unsigned int chnl);
	void NextFrame(unsigned int chnl);
	void SetIntensity(unsigned int chnl, float i) { chnl_intensity[chnl] = i; }
	int GetAnim() { return anim[0]; }
	int GetFrame() { return curFrame[0]; }
	void DrawSkeleton(glm::mat4 global, Shader* shdr);

	FrameStates GetFrameCode(unsigned int chnl = 0);

	Skeleton* GetSkeleton() { return skeleton; }

	SkelMesh* Clone() { return new SkelMesh(*this); };

	glm::vec3 GetRootMv();
};
