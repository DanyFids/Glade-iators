#include "Effect.h"
#include<Mesh.h>
#include<Texture.h>
#include<Hitbox.h>
#include"Light.h"
#include"Lerp.h"

void Effect::Die(std::vector<Effect*> &effect_list)
{
	for (int c = 0; c < effect_list.size(); c++) {
		if (this == effect_list.at(c)) {
			effect_list.erase(effect_list.begin() + c);
			break;
		}
	}
}

bool Effect::Cull()
{
	return life <= 0.0f;
}