#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <GLM/glm.hpp>
#include<GLM/gtc/matrix_transform.hpp>
#include <map>

struct Char {
	int TextureID;  // ID handle of the glyph texture
	glm::vec2 Size;       // Size of glyph
	glm::vec2 Bearing;    // Offset from baseline to left/top of glyph
	int Advance;    // Offset to advance to next glyph
};

class TextRenderer
{
	TextRenderer(bool Bold);

	void RenderText();
	glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);


};


