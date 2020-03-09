#include "Text.h"
#include <glad\glad.h>
#include "Game.h"


TextRenderer::TextRenderer()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "Fonts/Aetherius.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	FT_Set_Pixel_Sizes(face, 0, 48);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Char character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Chars.insert(std::pair<GLchar, Char>(c, character));
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	//Chars = Chars;
}

std::string TextRenderer::GenerateName()
{

	std::string Titles[21] = { "Weeping", "r/", "Looks", "Sir","Lord","Lady","Dr.","Sapling","Pinecone","Twig","Maple","Lorax", "Sonofabirch", "Nut", "Punished", "Sage", "Your", 
		"God Emperor", " Grand Champion", "It Was I,", "Seasoned" };

	std::string FirstNames[47] = { " Oberon", " Titania", " Fern", " Woody", " Spruce", " Willow", " Chris", " Chest", " Wal",
		" Pistachio", " Nice", " Gavin", " Hoity", " James", " Hoguey", " Dimitree", " Herbert"," Birchrude", " Leaf"," Cherry", " Paper", " Sugar", " Cedarella", " Arbor", 
		" Craig", " Dylan", " Maija", " Tia", " Dan", " Anthony", " Elm-o", " Douglas", " Laurel", " Nut", "Keanu", " Poison", " Oaky", " Worst", " Buckeye", " Treebeard", 
		" Olive", " Holly", " Iris", " Dill", " Mint", " Mary", " Aloe"   };

	std::string LastNames[51] = { " Oakborn", " Barkley", " Barkwell", " Elderwood", " Springstein", " Aspin", " Guys", " Baby", " Hogue", " McOak", " Oaksson",
		" Robb", " Gottem", " Birch", " Ball", " Nut", " Pine", " Cottonwood", " Cane", " Sassafras", " Leafsson", " Soil", " Root", " Rootsson", " Greenheart", " Wisteria", 
		" Needle", " Leafs", " Sugar", " Enemy", " Rowan", " Asher", " Cassia", " Flora", " Peachy",  " Elmore", " Hazelton", " Speedwagon", " Gladed", " Foliage", " Flowers", 
		" Peppervine", " Honeysuckle", " Peppercorn", " Doe", " Tarragon", " Wana", " Chervil", " Vera", " Catnip", "Rhubarb" };

	std::string Suffixs[23] = { " I", " II", " III", " IV", " V", " VI", " VII", " VIII", " The Broken", " The Unburnt",
		" The Chipped"," The Breaker of Branches"," The Mothertrunker", " The Fertilizer"," The Woodcutter", " Lord of Ashes", " The Dude", " Speaks for the trees", " The Damned", 
		" The Nut", " The Spicy", " The Abyss Watcher", " The Dragontree Born" };
	

	int Title = rand() % 20;
	int FirstName = rand() % 46;
	int LastName = rand() % 50;
	int Suffix = rand() % 22;

	std::string NAME = Titles[Title];
	NAME += FirstNames[FirstName];
	NAME += LastNames[LastName];
	NAME += Suffixs[Suffix];



	//std::string NAME = Titles[Title];
	//NAME.append(FirstNames[FirstName]);
	//NAME.append(LastNames[LastName]);
	//NAME.append(Suffixs[Suffix]);

	return NAME;
}

Shader* TextRenderer::TEXTSHADER = nullptr;
glm::mat4 TextRenderer::projection = glm::mat4(1.0f);

void TextRenderer::RenderText(Shader* s, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
{
	
///////////////////////////////////////////////////////////

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	s->Use();
	glUniform3f(glGetUniformLocation(s->ID, "TextColor"), color.x, color.y, color.z);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	s->SetMat4("projection", projection);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Char character = Chars[*c];

		GLfloat xpos = x + character.Bearing.x * scale;
		GLfloat ypos = y - (character.Size.y - character.Bearing.y) * scale;

		GLfloat width = character.Size.x * scale;
		GLfloat height = character.Size.y * scale;

		//update VBOsf
		GLfloat vertices[6][4] = {
			{xpos, ypos + height, 0.0, 0.0},
			{xpos, ypos, 0.0, 1.0 },
			{xpos + width, ypos, 1.0, 1.0},

			{xpos, ypos + height, 0.0, 0.0},
			{xpos + width, ypos, 1.0, 1.0},
			{xpos + width, ypos + height, 1.0, 0.0}
		};
		//render glyph texture
		glBindTexture(GL_TEXTURE_2D, character.TextureID);
		//update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (character.Advance >> 6) * scale; //bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::INIT()
{
	TEXTSHADER = new Shader("Shaders/Text.vert", "Shaders/Text.frag");
	projection = glm::ortho(0.f, float(Game::SCREEN.x), 0.f, float(Game::SCREEN.y), -1.f, 1.f);
}
