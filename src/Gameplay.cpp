#include "Gameplay.h"

Object* Attack::ABox = nullptr;

Attack::Attack(unsigned int P)
{
	player = P;
	time = 2;
}

void Attack::init() {
	Mesh* Amesh = new Mesh("d6.obj");
	Material* Amat = new Material("missing_tex.png");
	Hitbox* basicCubeHB = new CubeHitbox(1.0f, 1.0f, 1.0f);

	ABox = new Object(Amesh, Amat, basicCubeHB);
}

