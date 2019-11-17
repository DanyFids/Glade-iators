#pragma once
#include "Object.h"
#include "Mesh.h"
#include "Texture.h"
#include "Hitbox.h"

class Attack
{
private:

public:
	static Object* ABox;
	int player;
	float time;

	Attack(unsigned int P);

	void init();
};




