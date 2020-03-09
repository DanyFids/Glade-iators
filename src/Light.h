#pragma once
#include<GLM/glm.hpp>
#include<vector>
class Shader;
class Scene;

class Light {
protected:
	glm::vec3 position;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float ambient_strength;
	float diffuse_strength;
	float specular_strength;

	float far_plane;

	unsigned int depthMapFBO;
	unsigned int depthMap;
	glm::mat4 lightProjection;
	glm::mat4 lightView;

	void SetupOccRender();
public:
	Light(glm::vec3 pos, bool orth = true);
	Light(glm::vec3 pos, glm::vec3 color, float a, float d, float s, bool orth = true);
	Light(glm::vec3 pos, glm::vec3 ambi, glm::vec3 diff, glm::vec3 spec, float a, float d, float s, bool orth = true);

	void SetUniformColor(glm::vec3 color);
	void SetAmbient(glm::vec3 color, float str);
	void SetDiffuse(glm::vec3 color, float str);
	void SetSpecular(glm::vec3 color, float str);

	void SetupDepthPerspective(bool ortho);

	void Move(glm::vec3 dir) { position += dir; }
	void SetPos(glm::vec3 d) { position = d; }

	unsigned int GetFrameBuffer() { return depthMapFBO; };
	unsigned int GetDepthMap() { return depthMap; }

	void SetStr(float a, float d, float s);

	virtual void SetupLight(Shader* shader);
	virtual void SetupDepthShader(Shader* shader);
};

class DirectionalLight : public Light {

public:
	DirectionalLight(glm::vec3 pos);
	DirectionalLight(glm::vec3 pos, glm::vec3 color, float a, float d, float s);
	DirectionalLight(glm::vec3 pos, glm::vec3 ambi, glm::vec3 diff, glm::vec3 spec, float a, float d, float s);

	virtual void SetupLight(Shader* shader) override;
};

class Mesh;
class Material;

class PointLight : public Light {
protected:
	static Mesh* VOLUME;
	static Material* VOL_MAT;

	float constant;
	float linear;
	float quadratic;
	float radius;

	std::vector<glm::mat4> lightTransforms;

	void SetupCubeMap();
public:
	static void INIT();

	PointLight(glm::vec3 pos);
	PointLight(glm::vec3 pos, glm::vec3 color, float a, float d, float s, float l,float q, float c = 1.0f);
	PointLight(glm::vec3 pos, glm::vec3 ambi, glm::vec3 diff, glm::vec3 spec, float a, float d, float s, float l, float q, float c = 1.0f);

	virtual void SetupLight(Shader* shader);
	void SetupLight(Shader* shader, int num);
	virtual void SetupDepthShader(Shader* shader);
	virtual void Draw(Shader*);

	void SetIntensity(float l, float q);
};