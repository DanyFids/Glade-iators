#include "Particle.h"
#include <GLM/gtc/matrix_transform.hpp>
#include<GLM/gtc/quaternion.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Game.h"
#include "Camera.h"

const int ParticleEngine::MAX_PARTICLES = 2046;

void Particle::FireUpdate(float dt, Particle &p)
{
	p.position + p.velocity * dt;
	p.life -= dt;
	p.size = p.start_size * (p.life / p.lifetime);
}

void Particle::AudienceUpdate(float dt, Particle& p)
{
	p.life -= dt;
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
	std::vector<Part> points;
	for (int p = 0; p < MAX_PARTICLES; p++) {
		points.push_back(Part());
	}

	glGenVertexArrays(1, &PARTICLE_VAO);
	glBindVertexArray(PARTICLE_VAO);

	glGenBuffers(1, &PARTICLE_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, PARTICLE_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Part) * points.size(), points.data(), GL_DYNAMIC_DRAW);

	Part* temp = nullptr;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Part), &(temp->pos));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Part), &(temp->size));
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(2, 1, GL_INT, sizeof(Part), &(temp->variation));
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(3, 1, GL_INT, sizeof(Part), &(temp->frame));
	glEnableVertexAttribArray(3);
}

ParticleEngine::ParticleEngine(glm::vec3 position, glm::vec2 size, int max, float l, std::vector<std::vector<Material*>> mat, ParticleEngineBehavior eb, ParticleBehavior pb)
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
		particles[c].Update(dt);
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

	std::vector<Part> parts = {};
	for (int p = 0; p < particles.size(); p++) {
		parts.push_back({particles[p].position, particles[p].size, particles[p].variation, particles[p].frame});
	}

	int id = 0;
	for (int v = 0; v < Tex.size(); v++) {
		for (int f = 0; f < Tex[v].size(); f++) {
			glActiveTexture(GL_TEXTURE0 + id);
			glBindTexture(GL_TEXTURE_2D, Tex[v][f]->DIFF);
			shdr->SetI("tex[" + std::to_string(v) + "][" + std::to_string(f) + "]", id);
			id++;
		}
	}
	
	glBindVertexArray(PARTICLE_VAO);
	glBindBuffer(GL_ARRAY_BUFFER, PARTICLE_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Part) * parts.size(), parts.data(), GL_DYNAMIC_DRAW);
	glDrawArrays(GL_POINTS, 0, parts.size());
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

void ParticleEngine::AudienceEngineBehavior(float dt, ParticleEngine& e)
{
	if (e.e_init) {
		std::vector<glm::vec3> section_positions, particle_pos;
		
		// rows of 5 (1-3)
		// 1
		{
			glm::vec3 tmp = glm::vec3(-48.17f, 4.6392f, 0.0f);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
		}
		// 2
		{
			glm::vec3 tmp = glm::vec3(-51.043f, 5.7355f, 0.0f);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
		}
		// 3
		{
			glm::vec3 tmp = glm::vec3(-53.948f, 6.8877f, 0.0f);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
		}
		// 6 rows (4-6)
		// 4
		{
			glm::vec3 tmp = glm::vec3(-56.822f, 7.984f, 0.0f);
			tmp.z = -2.2912f;
			section_positions.push_back(tmp);
			tmp.z = -1.1466f;
			section_positions.push_back(tmp);
			tmp.z = -0.017152f;
			section_positions.push_back(tmp);
			tmp.z = 1.1152f;
			section_positions.push_back(tmp);
			tmp.z = 2.2263f;
			section_positions.push_back(tmp);
			tmp.z = 3.3972f;
			section_positions.push_back(tmp);
		}
		// 5
		{
			glm::vec3 tmp = glm::vec3(-59.874f, 9.0191f, 0.0f);
			tmp.z = -2.2912f;
			section_positions.push_back(tmp);
			tmp.z = -1.1466f;
			section_positions.push_back(tmp);
			tmp.z = -0.017152f;
			section_positions.push_back(tmp);
			tmp.z = 1.1152f;
			section_positions.push_back(tmp);
			tmp.z = 2.2263f;
			section_positions.push_back(tmp);
			tmp.z = 3.3972f;
			section_positions.push_back(tmp);
		}
		// 6
		{
			glm::vec3 tmp = glm::vec3(-62.829f, 10.117f, 0.0f);
			tmp.z = -2.2912f;
			section_positions.push_back(tmp);
			tmp.z = -1.1466f;
			section_positions.push_back(tmp);
			tmp.z = -0.017152f;
			section_positions.push_back(tmp);
			tmp.z = 1.1152f;
			section_positions.push_back(tmp);
			tmp.z = 2.2263f;
			section_positions.push_back(tmp);
			tmp.z = 3.3972f;
			section_positions.push_back(tmp);
		}
		// 7 rows (7-9)
		// 7
		{
			glm::vec3 tmp = glm::vec3(-65.751f, 11.278f, 0.0f);
			tmp.z = -2.8381f;
			section_positions.push_back(tmp);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
			tmp.z = 3.9482f;
			section_positions.push_back(tmp);
		}
		// 8
		{
			glm::vec3 tmp = glm::vec3(-68.764f, 12.332f, 0.0f);
			tmp.z = -2.8381f;
			section_positions.push_back(tmp);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
			tmp.z = 3.9482f;
			section_positions.push_back(tmp);
		}
		// 9
		{
			glm::vec3 tmp = glm::vec3(-71.651f, 13.435f, 0.0f);
			tmp.z = -2.8381f;
			section_positions.push_back(tmp);
			tmp.z = -1.6743f;
			section_positions.push_back(tmp);
			tmp.z = -0.52971f;
			section_positions.push_back(tmp);
			tmp.z = 0.59977f;
			section_positions.push_back(tmp);
			tmp.z = 1.7321f;
			section_positions.push_back(tmp);
			tmp.z = 2.8433f;
			section_positions.push_back(tmp);
			tmp.z = 3.9482f;
			section_positions.push_back(tmp);
		}
		// 8 rows (10-11)
		// 10
		{
			glm::vec3 tmp = glm::vec3(-74.605f, 14.563f, 0.0f);
			tmp.z = -3.4014f;
			section_positions.push_back(tmp);
			tmp.z = -2.2579f;
			section_positions.push_back(tmp);
			tmp.z = -1.1298f;
			section_positions.push_back(tmp);
			tmp.z = 0.014786f;
			section_positions.push_back(tmp);
			tmp.z = 1.1443f;
			section_positions.push_back(tmp);
			tmp.z = 2.2766f;
			section_positions.push_back(tmp);
			tmp.z = 3.3877f;
			section_positions.push_back(tmp);
			tmp.z = 4.5204f;
			section_positions.push_back(tmp);
		}
		// 11
		{
			glm::vec3 tmp = glm::vec3(-77.353f, 15.642f, 0.0f);
			tmp.z = -3.4014f;
			section_positions.push_back(tmp);
			tmp.z = -2.2579f;
			section_positions.push_back(tmp);
			tmp.z = -1.1298f;
			section_positions.push_back(tmp);
			tmp.z = 0.014786f;
			section_positions.push_back(tmp);
			tmp.z = 1.1443f;
			section_positions.push_back(tmp);
			tmp.z = 2.2766f;
			section_positions.push_back(tmp);
			tmp.z = 3.3877f;
			section_positions.push_back(tmp);
			tmp.z = 4.5204f;
			section_positions.push_back(tmp);
		}

		// Oh boy get ready!
		glm::vec3 rot = glm::vec3(0.0f, 12.4f, 0.0f);
		
		for (int s = 0; s < 29; s++) {
			glm::vec3 tmp_rot = rot * (float)s;
			glm::mat3 rot_mat = glm::mat3_cast(glm::angleAxis(glm::radians(tmp_rot.y), glm::vec3(0.0f, 1.0f, 0.0f)));

			for (int c = 0; c < section_positions.size(); c++) {
				glm::vec3 tmp = rot_mat * section_positions[c];

				float start_l = e.particle_l * (float)(rand() % 10 + 1) / 10.0f;

				particle_pos.push_back(tmp);
				Particle temp = Particle(tmp, glm::vec3(), start_l, e.particle_size, e.particle_b);

				int variation = rand() % e.Tex.size();
				int start_f = rand() % e.Tex[variation].size();

				temp.variation = variation;
				temp.frame = start_f;

				e.particles.push_back(temp);
			}
		}

		e.e_init = false;
	}
	else {	
		for (int p = 0; p < e.particles.size(); p++) {
			if (e.particles[p].life <= 0.0f) {
				e.particles[p].frame = e.particles[p].frame + 1;
				if (e.particles[p].frame > 3) {
					e.particles[p].frame = 0;
				}

				//e.particles[p].frame = (e.particles[p].frame < e.Tex[e.particles[p].variation].size() - 1) ? e.particles[p].frame++ : 0;
				e.particles[p].life = e.particle_l;
			}
		}
	}
}
