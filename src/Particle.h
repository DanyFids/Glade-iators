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

struct Part {
	glm::vec3 pos;
	glm::vec2 size;
	int variation;
	int frame;
};

class Particle {
protected:
	glm::vec3 position, velocity;
	glm::vec2 size, start_size;
	float life, lifetime;
	int variation = 0, frame = 0;

	ParticleBehavior behavior;

public:
	//Particle(glm::vec3 pos, glm::vec3 vel, float l) :position(pos), velocity(vel), life(l) {};
	Particle(glm::vec3 pos, glm::vec3 vel, float l, glm::vec2 s, ParticleBehavior b);

	virtual void Update(float dt);

	friend class ParticleEngine;

	// Behaviors
	static void FireUpdate(float dt, Particle& p);
	static void AudienceUpdate(float dt, Particle& p);
	static void RockUpdate(float dt, Particle& p);
};

class ParticleEngine {
	std::vector<Particle> particles;
	std::vector<std::vector<Material*>> Tex;

	int max_particles;

	glm::vec3 origin;
	glm::vec2 particle_size;
	float particle_l;

	bool e_init = true;

	ParticleEngineBehavior engine_b;
	ParticleBehavior particle_b;
public:
	static Mesh* Billboard;
	static Shader* PART_SHADER;
	static unsigned int PARTICLE_VBO;
	static unsigned int PARTICLE_VAO;
	static const int MAX_PARTICLES;

	static void INIT();

	ParticleEngine(glm::vec3 positions, glm::vec2 size, int max, float l, std::vector<std::vector<Material*>> mat, ParticleEngineBehavior eb, ParticleBehavior pb);

	virtual void Update(float dt);
	virtual void Draw(Camera* Cam);
	virtual void UpdateMaxParticles(int density);

	// behaviors
	static void FireEngineBehavior(float dt, ParticleEngine& e);
	static void AudienceEngineBehavior(float dt, ParticleEngine& e);
	static void RockEngine(float dt, ParticleEngine& e);
};