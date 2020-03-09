#pragma once
#include "Game.h"
#include <vector>
#include<string>

class Mesh;
class FrameBuffer;
class Shader;
class LUT;

class PostProcess {
protected:
	std::vector<unsigned int> IN;

	Shader* effect; 

public:
	FrameBuffer* buff;

	PostProcess(std::vector<unsigned int> inputs, Shader* effect = nullptr);

	virtual void Draw();
};

class LutColorCorrection : public PostProcess {
protected:
	LUT* lut;

public:
	static Shader* SHADER;

	LutColorCorrection(LUT* l, unsigned int input);

	virtual void Draw();
};

class FrameBuffer {
private:
	unsigned int ID;
	 std::vector<unsigned int> OUT;
	unsigned int DEPTH;

public:
	FrameBuffer();
	~FrameBuffer();

	void Use();
	void Clear();

	unsigned int GetBuffer() { return ID; }
	unsigned int GetOutput(int c = 0) { return OUT[c]; }
	unsigned int GetDepth() { return DEPTH; }
	void AddComponent();

	void Resize();
};