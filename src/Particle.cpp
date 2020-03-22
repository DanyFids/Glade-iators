#include "Particle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Camera.h"


void Particle::FireUpdate(float dt, Particle &p)
{
	p.position + p.velocity * dt;
	p.life -= dt;
	p.size = p.start_size * (p.life / p.lifetime);
}

Particle::Particle(glm::vec3 pos, glm::vec3 vel, float l, glm::vec2 s, ParticleBehavior b)
{
	position = pos;
	velocity = vel;
	lifetime = life = l;
	start_size = size = s;
	behavior = b;
}

void Particle::Update(float dt)
{
	std::invoke(behavior.func, dt, *this);
}

unsigned int ParticleEngine::PARTICLE_VBO = 0;
unsigned int ParticleEngine::PARTICLE_VAO = 0;

void ParticleEngine::INIT()
{
	std::vector<glm::vec3> points;
	for (int p = 0; p < 512; p++) {
		points.push_back(glm::vec3(0.0f));
	}

	glGenVertexArrays(1, &PARTICLE_VAO);
	glBindVertexArray(PARTICLE_VAO);

	glGenBuffers(1, &PARTICLE_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, PARTICLE_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * points.size(), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(0);
}

ParticleEngine::ParticleEngine(glm::vec3 position, glm::vec2 size, int max, float l, Material* mat, ParticleEngineBehavior eb, ParticleBehavior pb)
{
	max_particles = max;
	engine_b = eb;
	particle_b = pb;
	particle_l = l;
	Tex = mat;
	particle_size = size;

	origin = position;
}

void ParticleEngine::Update(float dt)
{
	std::invoke(engine_b.func, dt, *this);

	for (int c = 0; c < particles.size(); c++) {
		//particles[c].Update(dt);
	}
}

Mesh* ParticleEngine::Billboard = nullptr;
Shader* ParticleEngine::PART_SHADER = nullptr;

void ParticleEngine::Draw(Camera* Cam)
{	
	if (Billboard == nullptr) {
		float quad_prim[] = {
			// x, y, z, r, g, b, u, v
			-0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
			 0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f, 0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
			 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f,
			-0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,  0.0f,0.0f,0.0f,0.0f,0.0f,0.0f,1.0f
		};

		unsigned int quad_index[] = {
			0, 1, 2,
			0, 2, 3
		};

		ParticleEngine::Billboard = new Mesh(quad_prim, 4, quad_index, 6);
	}

	if (PART_SHADER == nullptr) {
		PART_SHADER = new Shader("Shaders/Particle.vert", "Shaders/Particle.frag", "Shaders/Particle.geom");
		INIT();
	}

	Shader* shdr = PART_SHADER;

	shdr->Use();
	shdr->SetVec3("cam_r", Cam->GetRight());
	shdr->SetVec3("cam_u", Cam->GetUp());
	shdr->SetMat4("VP_mat", Cam->GetProject() * Cam->GetLookAt());

	shdr->SetI("num_particles", particles.size());

	for (int p = 0; p < particles.size(); p++) {
		std::string name = "[" + std::to_string(p) + "]";
		shdr->SetVec3("pos" + name, particles[p].position);
		shdr->SetVec2("size" + name, particles[p].size);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Tex->DIFF);
	shdr->SetI("tex", 0);
	
	glBindVertexArray(PARTICLE_VAO);
	glDrawArrays(GL_POINTS, 0, particles.size());
	glBindVertexArray(0);
}

void ParticleEngine::FireEngineBehavior(float dt, ParticleEngine& e)
{
	for (int p = 0; p < e.particles.size(); p++) {
		if (e.particles[p].life <= 0.0f) {
			e.particles.erase(e.particles.begin() + p);
			p--;
			continue;
		}
	}

	while (e.particles.size() < e.max_particles) {
		float r = (float)(std::rand() % 3 + 1) / 2.0f;

		glm::vec3 vel = glm::vec3(0.0f, r, 0.0f);

		e.particles.push_back(Particle(e.origin, vel, e.particle_l + r, e.particle_size, e.particle_b));
		e.particles.push_back(Particle(e.origin + glm::vec3(1.0f, 0.0f, 0.0f), vel, e.particle_l + r, e.particle_size, e.particle_b));
		e.particles.push_back(Particle(e.origin + glm::vec3(-1.0f, 0.0f, 0.0f), vel, e.particle_l + r, e.particle_size, e.particle_b));
		e.particles.push_back(Particle(e.origin + glm::vec3(0.0f, 0.0f, 1.0f), vel, e.particle_l + r, e.particle_size, e.particle_b));
	}
}
