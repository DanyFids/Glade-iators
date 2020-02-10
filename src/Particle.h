#include<GLM/glm.hpp>
#include<functional>

class Mesh;
class Material;

class Particle {
protected:
	static Mesh* Quad;
	Material* Tex;
	glm::vec3 position, velocity;
	float size;
	float life;

	std::function<void(float)> behavior;

public:
	static void init();

	//Particle(glm::vec3 pos, glm::vec3 vel, float l) :position(pos), velocity(vel), life(l) {};
	Particle(Material* mat, glm::vec3 pos, glm::vec3 vel, float l, float s, std::function<void(float)> b);

	virtual void Update(float dt);
};