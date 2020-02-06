#pragma once
#include<GLM/glm.hpp>

float lerp(float f1, float f2, float t);

glm::vec3 lerp(glm::vec3 p1, glm::vec3 p2, float t);

glm::mat4 lerp(glm::mat4 p1, glm::mat4 p2, float t);