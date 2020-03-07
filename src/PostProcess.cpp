#include "PostProcess.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"

FrameBuffer::FrameBuffer()
{
	// Generate Buffers
	glGenFramebuffers(1, &ID);
	glGenTextures(1, &OUT);
	glGenTextures(1, &DEPTH);

	//Setup Main Color Output
	glBindTexture(GL_TEXTURE_2D, OUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Game::SCREEN.x, Game::SCREEN.y, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// Setup Depth Image
	glBindTexture(GL_TEXTURE_2D, DEPTH);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Game::SCREEN.x, Game::SCREEN.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, OUT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DEPTH, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer() {
	glDeleteFramebuffers(1, &ID);
	glDeleteTextures(1, &OUT);
}

void FrameBuffer::Use()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FrameBuffer::Clear()
{
	Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameBuffer::Resize()
{
	glBindTexture(GL_TEXTURE_2D, OUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Game::SCREEN.x, Game::SCREEN.y, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, DEPTH);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, Game::SCREEN.x, Game::SCREEN.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

PostProcess::PostProcess(std::vector<unsigned int> inputs, Shader* effect)
{
	IN = inputs;
	this->effect = effect;
	buff = new FrameBuffer();
}

void PostProcess::Draw()
{
	effect->Use();

	for (int c = 0; c < IN.size(); c++) {
		glBindTexture(GL_TEXTURE0 + c, IN[c]);
		effect->SetI("INPUT_" + std::to_string(c), c);
	}

	buff->Use();

	Game::QUAD->Draw(effect);
}

Shader* LutColorCorrection::SHADER = nullptr;

LutColorCorrection::LutColorCorrection(LUT* l, unsigned int input) : PostProcess({ input })
{
	if (LutColorCorrection::SHADER == nullptr) {
		LutColorCorrection::SHADER = new Shader("Shaders/PostProcess/PostProcess.vert", "Shaders/PostProcess/LUT_CC.frag");
	}

	lut = l;
	effect = SHADER;
}

void LutColorCorrection::Draw()
{
	effect->Use();

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_3D, lut->ID);
	effect->SetI("lut", 11);

	effect->SetI("lut_size", lut->LUT_SIZE);

	for (int c = 0; c < IN.size(); c++) {
		glActiveTexture(GL_TEXTURE0 + c);
		glBindTexture(GL_TEXTURE_2D, IN[c]);
		effect->SetI("INPUT_" + std::to_string(c), c);
	}

	buff->Use();

	Game::QUAD->Draw(effect);
}
