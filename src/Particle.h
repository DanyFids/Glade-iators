#include<GLM/glm.hpp>
#include<functional>

class Mesh;
class Material;
class Camera;
class Shader;

class Particle;
class ParticleEngine;

struct ParticleBehavior { std::function<void(float, Particle&)> func; };
struct ParticleEngineBehavior { std::function<void(float, ParticleEngine&)> func; };

class Particle {
protected:
	glm::vec3 position, velocity;
	glm::vec2 size, start_size;
	float life, lifetime;

	ParticleBehavior behavior;

public:
	static void FireUpdate(float dt, Particle &p);

	//Particle(glm::vec3 pos, glm::vec3 vel, float l) :position(pos), velocity(vel), life(l) {};
	Particle(glm::vec3 pos, glm::vec3 vel, float l, glm::vec2 s, ParticleBehavior b);

	virtual void Update(float dt);

	friend class ParticleEngine;
};

class ParticleEngine {
	std::vector<Particle> particles;
	Material* Tex;

	int max_particles;

	glm::vec3 origin;
	glm::vec2 particle_size;
	float particle_l;

	ParticleEngineBehavior engine_b;
	ParticleBehavior particle_b;
public:
	static Mesh* Billboard;
	static Shader* PART_SHADER;
	static unsigned int PARTICLE_VBO;
	static unsigned int PARTICLE_VAO;
	static const int MAX_PARTICLES;

	static void INIT();

	ParticleEngine(glm::vec3 positions, glm::vec2 size, int max, float l, Material* mat, ParticleEngineBehavior eb, ParticleBehavior pb);

	virtual void Update(float dt);
	virtual void Draw(Camera* Cam);

	// behaviors
	static void FireEngineBehavior(float dt, ParticleEngine& e);
};