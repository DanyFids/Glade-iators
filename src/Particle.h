#include<GLM/glm.hpp>
#include<functional>

class Mesh;
class Material;
class Camera;
class Shader;

class Particle {
protected:
	Material* Tex;
	glm::vec3 position, velocity;
	glm::vec2 size;
	float life;

	std::function<void(float)> behavior;

public:

	//Particle(glm::vec3 pos, glm::vec3 vel, float l) :position(pos), velocity(vel), life(l) {};
	Particle(glm::vec3 pos, glm::vec3 vel, float l, glm::vec2 s, std::function<void(float)> b);

	virtual void Update(float dt);
};

class ParticleEngine {
	std::vector<Particle> particles;

	int max_particles;

	glm::vec3 origin;
	glm::vec2 particle_size;
	float particle_l;

	std::function<void(float, std::vector<Particle>)> engine_b;
	std::function<void(float)> particle_b;
public:
	ParticleEngine(glm::vec3 positions, int max, float l, std::function<void(float, std::vector<Particle>)> eb, std::function<void(float)> pb);

	virtual void Update(float dt);
	virtual void Draw(Shader* shdr, Camera* Cam);

};