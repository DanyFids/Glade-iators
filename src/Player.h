#pragma once

#include "Object.h"


class Player : Object
{
	int lives;
	float speed;

	void Move(glm::vec3 dir, float spd);

};