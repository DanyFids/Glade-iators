#include "Particle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"


Particle::Particle(glm::vec3 pos, glm::vec3 vel, float l, glm::vec2 s, std::function<void(float)> b)
{
}

void Particle::Update(float dt)
{
	std::invoke(behavior, dt);
}

ParticleEngine::ParticleEngine(glm::vec3 position, int max, float l, std::function<void(float, std::vector<Particle>)> eb, std::function<void(float)> pb)
{
	max_particles = max;
	engine_b = eb;
	particle_b = pb;
	particle_l = l;

	origin = position;
}
