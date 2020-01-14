#pragma once
#include<GLM/glm.hpp>
#include<vector>
#include<string>

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
	void Update(float dt);

	void LoadMesh(const char* f, std::vector<Vertex> &vertices, unsigned int& num_verts, std::vector<unsigned int> &indices, unsigned int& num_indi);

	void Move(glm::vec3 dir, float dt);
	void SetPosition(glm::vec3 pos);
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
	int curFrame;
	int nexFrame;
	int anim = 0;

	Skeleton* skeleton;
	//Material* weightMap;

public:
	SkelMesh(std::string f, Skeleton* s, std::string weightMap);
	~SkelMesh();

	virtual void Draw(Shader*);
	void SetAnim(int id) { anim = id; }
	void SetFrame(int id) { curFrame = id; }
};