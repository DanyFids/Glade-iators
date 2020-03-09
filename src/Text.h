#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <map>
#include "Game.h"
#include "Shader.h"
#include <stdlib.h>

struct Char {
	GLint TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLint Advance;    // Offset to advance to next glyph
};

//class Shader;

class TextRenderer
{
public:
	TextRenderer();

	std::string GenerateTitle();
	std::string GenerateSuffix();
	std::string GenerateName();

	static Shader* TEXTSHADER;
	std::map<GLchar, Char> Chars;
	void RenderText(Shader* s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
	static glm::mat4 projection;
	static void INIT();

};


