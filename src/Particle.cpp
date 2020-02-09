#include "Particle.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
        
Mesh* Particle::Quad = nullptr;

void Particle::init()
{                                                   
	Quad = new Mesh("pane.obj");
}

Particle::Particle(Material* mat, glm::vec3 pos, glm::vec3 vel, float l, float s, std::function<void(float)> b)
{
	Tex = mat;
	position = pos;
	velocity = vel;

	life = l;
	size = s;

	behavior = b;
}

void Particle::Update(float dt)
{
	std::invoke(behavior, dt);
}
