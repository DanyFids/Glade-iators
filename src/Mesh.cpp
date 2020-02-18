#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <stb_image.h>
#include <fstream>
#include <sstream>
#include<iostream>
#include "Game.h"
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Camera.h"
#include "Constants.h"
#include "Object.h"
#include "Skeleton.h"

Mesh::Mesh() {
}

//Hardcoded Loader
Mesh::Mesh(float vert[], int num_vert, unsigned int indi[], int num_indi) {

	// setup Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// setup array Buffer
	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * num_vert, vert, GL_STATIC_DRAW);

	// setup element buffer
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * num_indi, indi, GL_STATIC_DRAW);

	Vertex* temp_vert = nullptr;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->tex_uv));
	glEnableVertexAttribArray(2);

	num_indices = num_indi;

	// save buffers
	buffers[0] = vbo;
	buffers[1] = ebo;
	
	// Setup Transformation Matrices
	model = glm::mat4(1.0f); // world transformations
	//model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	//test transformations
	//transform = glm::rotate(transform, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
}

//Obj Loader
Mesh::Mesh(const char* file)
{
	unsigned int num_vert, num_indi;
	

	//std::vector<Vertex> vert_vec = std::vector<Vertex>();
	std::vector<unsigned int> indi_vec = std::vector<unsigned int>();

	LoadMesh(file, vertices, num_vert, indi_vec, num_indi);

	Vertex* vert;
	unsigned int* indi;
	vert = new Vertex[num_vert];
	indi = new unsigned int[num_indi];

	for (int c = 0; c < vertices.size(); c++) {
		vert[c] = vertices[c];
	}

	for (int c = 0; c < indi_vec.size(); c++) {
		indi[c] = indi_vec[c];
	}

	// setup Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// setup array Buffer	
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * num_vert, vert, GL_STATIC_DRAW);

	// setup element buffer
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * num_indi, indi, GL_STATIC_DRAW);

	Vertex* temp_vert = nullptr;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->position));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->tex_uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->tangent));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &(temp_vert->biTan));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Vertex), &(temp_vert->id));
	glEnableVertexAttribArray(5);


	num_indices = num_indi;

	// save buffers
	buffers[0] = vbo;
	buffers[1] = ebo;

	// Setup Transformation Matrices
	model = glm::mat4(1.0f); // world transformations
}

Mesh::~Mesh()
{
	
}

void Mesh::Draw(Shader* shader)
{
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void Mesh::SetTexture(Material* t) {
	tex = t;
}

void Mesh::Update(float dt) {
	model = glm::rotate(model, glm::radians(50.0f * dt), glm::vec3(0.5f, 1.0f, 1.0f));
}

void Mesh::LoadMesh(const char* f, std::vector<Vertex>& vertices, unsigned int& num_verts, std::vector<unsigned int>& indices, unsigned int& num_indi)
{
	std::ifstream file;
	file.open(f, std::ios::in);

	std::vector<glm::vec3> positions = std::vector<glm::vec3>();
	std::vector<glm::vec3> norms;
	std::vector<glm::vec2> texUVs;

	//std::vector<Vertex> vertices;

	std::string line;

	//std::vector<int> indices;

	while (!file.eof()) {
		if (file.peek() == '#') {
			file.ignore(512, (int)'\n');
			continue;
		}
		file >> line;

		if (line.compare("v") == 0) {
			glm::vec3 temp;

			file >> line;
			temp.x = std::stof(line);
			file >> line;
			temp.y = std::stof(line);
			file >> line;
			temp.z = std::stof(line);

			positions.push_back(glm::vec3(temp.x, temp.y, temp.z));
		}
		else if(line.compare("vn") == 0){
			glm::vec3 temp;

			file >> line;
			temp.x = std::stof(line);
			file >> line;
			temp.y = std::stof(line);
			file >> line;
			temp.z = std::stof(line);

			norms.push_back(glm::vec3(temp.x, temp.y, temp.z));
		}
		else if (line.compare("vt") == 0) {
			glm::vec2 temp;

			file >> line;
			temp.x = std::stof(line);
			file >> line;
			temp.y = std::stof(line);

			texUVs.push_back(glm::vec2(temp.x, temp.y));
		}
		else if (line.compare("f") == 0) {
			int count = 0;
			int base = vertices.size();

			while (file.peek() != '\n') {
				file >> line;

				int pos, nor, tex;

				std::istringstream data(line);
				std::string i;
				std::getline(data, i, '/');
				pos = std::stoi(i);
				std::getline(data, i, '/');
				tex = std::stoi(i);
				data >> i;
				nor = std::stoi(i);

				Vertex temp;
				temp.position = positions[pos-1];
				temp.normal = norms[nor-1];
				temp.tex_uv = texUVs[tex-1];
				temp.id = pos - 1;

				vertices.push_back(Vertex(temp));
				count++;
			}

			if (count == 3) {
				indices.push_back(base);
				indices.push_back(base+1);
				indices.push_back(base+2);

				// calculating tangenty and bitangent lines
				glm::vec3 e1 = vertices[base + 1].position - vertices[base].position;
				glm::vec3 e2 = vertices[base + 2].position - vertices[base].position;
				glm::vec2 d1 = vertices[base + 1].tex_uv - vertices[base].tex_uv;
				glm::vec2 d2 = vertices[base + 2].tex_uv - vertices[base].tex_uv;

				glm::vec3 tangent;
				glm::vec3 biTan;

				float fac = 1.0f / (d1.x * d2.y - d2.x * d1.y);

				tangent.x = fac * (d2.y * e1.x - d1.y * e2.x);
				tangent.y = fac * (d2.y * e1.y - d1.y * e2.y);
				tangent.z = fac * (d2.y * e1.z - d1.y * e2.z);
				tangent = glm::normalize(tangent);

				biTan.x = fac * (-d2.x * e1.x + d1.x * e2.x);
				biTan.y = fac * (-d2.x * e1.y + d1.x * e2.y);
				biTan.z = fac * (-d2.x * e1.z + d1.x * e2.z);
				biTan = glm::normalize(biTan);

				vertices[base].tangent = tangent;
				vertices[base].biTan = biTan;
				vertices[base+1].tangent = tangent;
				vertices[base+1].biTan = biTan;
				vertices[base+2].tangent = tangent;
				vertices[base+2].biTan = biTan;
			}
			else if (count == 4) {
				indices.push_back(base);
				indices.push_back(base + 1);
				indices.push_back(base + 2);

				indices.push_back(base + 0);
				indices.push_back(base + 2);
				indices.push_back(base + 3);

				// calculating tangenty and bitangent lines
				// Tri 1
				glm::vec3 e1 = vertices[base].position - vertices[base+1].position;
				glm::vec3 e2 = vertices[base + 2].position - vertices[base+1].position;
				glm::vec2 d1 = vertices[base].tex_uv - vertices[base+1].tex_uv;
				glm::vec2 d2 = vertices[base + 2].tex_uv - vertices[base+1].tex_uv;

				glm::vec3 tangent1;
				glm::vec3 biTan1;

				float fac1 = 1.0f / (d1.x * d2.y - d2.x * d1.y);

				tangent1.x = fac1 * (d2.y * e1.x - d1.y * e2.x);
				tangent1.y = fac1 * (d2.y * e1.y - d1.y * e2.y);
				tangent1.z = fac1 * (d2.y * e1.z - d1.y * e2.z);
				tangent1 = glm::normalize(tangent1);

				biTan1.x = fac1 * (-d2.x * e1.x + d1.x * e2.x);
				biTan1.y = fac1 * (-d2.x * e1.y + d1.x * e2.y);
				biTan1.z = fac1 * (-d2.x * e1.z + d1.x * e2.z);
				biTan1 = glm::normalize(biTan1);

				// Tri 1
				glm::vec3 e3 = vertices[base].position - vertices[base+3].position;
				glm::vec3 e4 = vertices[base + 2].position - vertices[base+3].position;
				glm::vec2 d3 = vertices[base].tex_uv - vertices[base+3].tex_uv;
				glm::vec2 d4 = vertices[base + 2].tex_uv - vertices[base+3].tex_uv;

				glm::vec3 tangent2;
				glm::vec3 biTan2;

				float fac2 = 1.0f / (d3.x * d4.y - d4.x * d3.y);

				tangent2.x = fac2 * (d4.y * e3.x - d3.y * e4.x);
				tangent2.y = fac2 * (d4.y * e3.y - d3.y * e4.y);
				tangent2.z = fac2 * (d4.y * e3.z - d3.y * e4.z);
				tangent2 = glm::normalize(tangent2);

				biTan2.x = fac2 * (-d4.x * e3.x + d3.x * e4.x);
				biTan2.y = fac2 * (-d4.x * e3.y + d3.x * e4.y);
				biTan2.z = fac2 * (-d4.x * e3.z + d3.x * e4.z);
				biTan2 = glm::normalize(biTan2);

				vertices[base].tangent = (tangent1 + tangent2) * 0.5f;
				vertices[base].biTan = (biTan1 + biTan2) * 0.5f;
				vertices[base + 1].tangent = tangent1;
				vertices[base + 1].biTan = biTan1;
				vertices[base + 2].tangent = (tangent1 + tangent2) * 0.5f;
				vertices[base + 2].biTan = (biTan1 + biTan2) * 0.5f;
				vertices[base + 3].tangent = tangent2;
				vertices[base + 3].biTan = biTan2;

				// NOTE:: Vertex 0 and Vertex 2 are shared between both tris, so their tangents and bitangents are averaged out just in case.
			}
		}
		else {
			file.ignore(512, (int)'\n');
		}
	}
	num_verts = vertices.size();
	num_indi = indices.size();
}

void Mesh::Move(glm::vec3 dir, float dt)
{
	model = glm::translate(model, dir * PLAYER_SPEED * dt);
}

void Mesh::SetPosition(glm::vec3 pos)
{
	model = glm::translate(model, pos);
}

MorphMesh::MorphMesh(std::vector<std::string> keyframes)
{
	keyFrames = keyframes;
	num_frames = keyFrames.size();

	for (int i = 0; i < num_frames; i++) {
		Pose temp; // create a Pose

		temp.id = i; // give it an ID so debugging isnt stupid

		LoadMesh(keyFrames[i].c_str(), temp.vert_vec, temp.num_vert, temp.indi_vec, temp.num_indi); //load the things.

		poses.push_back(temp); // add it to a list of poses.
	}

	//Declaring variables
	unsigned int num_vert, num_vert2, num_indi;

	//Current Pose
	//std::vector<Vertex> vert_vec = std::vector<Vertex>();

	//Next pose
	std::vector<Vertex> vert_vec2 = std::vector<Vertex>();

	std::vector<unsigned int> indi_vec = std::vector<unsigned int>(); //Vector of indices

	//LoadMesh(keyFrames[curFrame].c_str(), vert_vec, num_vert, indi_vec, num_indi);

	num_indi = poses[curFrame].num_indi;

	//Vertex* vert;
	
	unsigned int* indi;

	//vert = new Vertex[num_vert];  
	indi = new unsigned int[num_indi];


	//for (int c = 0; c < vert_vec.size(); c++) {
	//	vert[c] = vert_vec[c];
	//}

	for (int c = 0; c < poses[curFrame].indi_vec.size(); c++) {
		indi[c] = poses[curFrame].indi_vec[c];
	}

	//LoadMesh(keyFrames[nextFrame].c_str(), vert_vec2, num_vert2, indi_vec, num_indi);

	if (poses[curFrame].num_vert != poses[nextFrame].num_vert) { // If pose 2 does not have = number of verts
		std::cout << "Uh oh, stinky" << std::endl;
		return;
	}

	Vertex* vert2 = new Vertex[poses[nextFrame].num_vert * 2];

	int id = 0;
	for (int c = 0; c < poses[nextFrame].vert_vec.size(); c++) {
		vert2[id++] = poses[curFrame].vert_vec[c];
		vert2[id++] = poses[nextFrame].vert_vec[c];
	}

	

	// setup Vertex Array Object
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// setup array Buffer
	//unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * poses[curFrame].num_vert * 2, vert2, GL_DYNAMIC_DRAW); //May need to have a bigger size?

	// setup element buffer
	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * poses[curFrame].num_indi, indi, GL_STATIC_DRAW);

	bigVert* temp_vert = nullptr;

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->posi1));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->norm1));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->tex_uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->tang1));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->biTa1));
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->posi2));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->norm2));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->tang2));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(8, 3, GL_FLOAT, GL_FALSE, sizeof(bigVert), &(temp_vert->biTa2));
	glEnableVertexAttribArray(8);

	num_indices = num_indi;
	// save buffers
	buffers[0] = vbo;
	buffers[1] = ebo;

	// Setup Transformation Matrices
	model = glm::mat4(1.0f); // world transformations
}

void MorphMesh::Draw(Shader* shdr)
{
	shdr->SetF("time", time / ANIM_TIME);

	Mesh::Draw(shdr);
}

void MorphMesh::addPose( std::string filename)
{
	keyFrames.push_back(filename);
}

void MorphMesh::Update(float dt)
{
	if (!paused) {

		if (!looping && !reversed) { //forwards

			if (time >= ANIM_TIME) {
				time = 0;
				NextPose();
			}

			if (time < ANIM_TIME) {
				time += dt;

				if (time > ANIM_TIME) {
					time = ANIM_TIME;
				}

			}
		}

		else if (!looping && reversed) { //backwards

			if (time <= 0) {
				time = ANIM_TIME;
				PrevPose();
			}

			if (time > 0) {
				time -= dt;

				if (time < 0.0f) {
					time = 0.0f;
				}
			}
		}


		else if (looping) {


		}

	}
	//time = 1.0f;
}

void MorphMesh::NextPose()
{
	curFrame = nextFrame;
	nextFrame++;

	if (nextFrame > num_frames - 1) {
		
		if (!looping) {
			setPose(0, 0.0f);
		}
		else
			nextFrame = 0;
	}

	Vertex* vert2 = new Vertex[poses[0].num_vert * 2];

	int id = 0;
	for (int c = 0; c < poses[0].vert_vec.size(); c++) {
		vert2[id++] = poses[curFrame].vert_vec[c];
		vert2[id++] = poses[nextFrame].vert_vec[c];
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * poses[curFrame].num_vert * 2, vert2, GL_DYNAMIC_DRAW);

}


void MorphMesh::PrevPose()
{
	
	nextFrame = curFrame;
	curFrame--;

	if (curFrame < 0) {
		
		if (!looping) {
			setPose(num_frames - 2, ANIM_TIME);
		}
		else
		{
			curFrame = num_frames - 1;
		}
	}

	Vertex* vert2 = new Vertex[poses[nextFrame].num_vert * 2];

	int id = 0;
	for (int c = 0; c < poses[curFrame].vert_vec.size(); c++) {
		vert2[id++] = poses[curFrame].vert_vec[c];
		vert2[id++] = poses[nextFrame].vert_vec[c];
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * poses[curFrame].num_vert * 2, vert2, GL_DYNAMIC_DRAW);
}


void MorphMesh::setLooping()
{
	if (!looping) {
		looping = true;
	}
	else
		looping = false;
}

void MorphMesh::setReverse()
{
	if (!reversed) {
		reversed = true;
		time = ANIM_TIME;
	}
	else {
		reversed = false;
		time = 0;
	}
}

void MorphMesh::setPose(int frame, float t)
{
	if (frame < 0 || frame > num_frames - 1) {
		std::cout << "Frames out of bounds!";
		curFrame = 0;
		nextFrame = 1;
		return;
	}

	curFrame = frame;
	nextFrame = curFrame + 1;

	if (t < 0 || t > ANIM_TIME) {
		std::cout << "Anim Time out of bound!";
		time = 0.0f;
		return;
	}

	time = t;

	Vertex* vert2 = new Vertex[poses[nextFrame].num_vert * 2];

	int id = 0;
	for (int c = 0; c < poses[curFrame].vert_vec.size(); c++) {
		vert2[id++] = poses[curFrame].vert_vec[c];
		vert2[id++] = poses[nextFrame].vert_vec[c];
	}

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * poses[curFrame].num_vert * 2, vert2, GL_DYNAMIC_DRAW);
}

void MorphMesh::play()
{
	paused = true;
}

void MorphMesh::pause()
{
	paused = true;
}

SkelMesh::SkelMesh(std::string f, Skeleton* s, std::string w): Mesh(f.c_str())
{
	skeleton = s;
	curFrame = 0;
	nexFrame = 0;

	float** weightMap;
	int verts;
	int joints;

	int nrChnls;
	unsigned char* temp = stbi_load(w.c_str(), &joints, &verts, &nrChnls, 0);

	weightMap = new float*[verts];

	for (int v = 0; v < verts; v++) {
		weightMap[v] = new float[joints];

		for (int j = 0; j < joints; j++) {
			weightMap[v][j] = (float)temp[j * nrChnls + v*joints*nrChnls] / 255.0f;

			//std::cout << weightMap[v][j] << " ";
		}
		//std::cout << std::endl;
	}

	std::vector<SkelVert> skel_verts;

	for each (Vertex vert in vertices) {
		SkelVert temp(vert);

		int bone_i[] = { 0,0,0,0 };
		float bone_w[] = { 0.0f, 0.0f, 0.0f, 0.0f };

		for (int j = 0; j < joints; j++) {
			float w = weightMap[vert.id][j];

			for (int c = 0; c < 4; c++) {
				if (w > bone_w[c]) {
					for (int b = 2; b >= c; b--) {
						bone_i[b + 1] = bone_i[b];
						bone_w[b + 1] = bone_w[b];
					}

					bone_i[c] = j;
					bone_w[c] = w;

					break;
				}
			}
		}

		float weight_total = bone_w[0] + bone_w[1] + bone_w[2] + bone_w[3];

		temp.bone_ids = glm::ivec4(bone_i[0], bone_i[1], bone_i[2], bone_i[3]);
		temp.weights = glm::vec4(bone_w[0] / weight_total, bone_w[1]/ weight_total, bone_w[2]/ weight_total, bone_w[3] / weight_total);

		skel_verts.push_back(temp);
	}
	
	SkelVert* temp_vert = nullptr;

	glBindVertexArray(vao);

	// Update Attrib Pointer Stride
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->position));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->normal));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->tex_uv));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->tangent));
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->biTan));
	glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(SkelVert), &(temp_vert->id));

	// add ids and weights
	glVertexAttribIPointer(6, 4, GL_INT, sizeof(SkelVert), &(temp_vert->bone_ids));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(SkelVert), &(temp_vert->weights));
	glEnableVertexAttribArray(7);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(SkelVert) * skel_verts.size(), skel_verts.data(), GL_STATIC_DRAW);
	
	//std::cout << "HOI!!!" << std::endl;
	for (int v = 0; v < verts; v++) {
		delete(weightMap[v]);
	}

	delete(weightMap);
}

SkelMesh::~SkelMesh()
{
	
}

void SkelMesh::Update(float dt)
{
	if (skeleton->anim_ft[anim] > 0.0f) {
		anim_time += dt;

		if (anim_time > skeleton->anim_ft[anim]) {
			NextFrame();
			anim_time = 0.0f;
		}
	}
}

void SkelMesh::Draw(Shader* shdr)
{
	glm::mat4* arr = nullptr;
	glm::mat4* axis = nullptr;
	glm::mat4* axis_i = nullptr;
	glm::vec3* binds = nullptr;
	glm::vec3* bind_t = nullptr;
	glm::mat3* norms = nullptr;
	skeleton->GetTransformArray(arr, axis, axis_i, norms, binds, bind_t, anim, curFrame);
	int num_b = skeleton->GetNumBones();

	//glActiveTexture(GL_TEXTURE20);
	//glBindTexture(GL_TEXTURE_2D, weightMap->DIFF);
	//shdr->SetI("weightMap", 20);
	
	shdr->SetI("num_bones", num_b);
	for (int b = 0; b < num_b; b++) {
		std::string b_string = std::to_string(b);
		shdr->SetMat4("bone_t[" + b_string + "]", arr[b]);
		shdr->SetVec3("bind_p[" + b_string + "]", binds[b]);
		shdr->SetVec3("bind_t[" + b_string + "]", bind_t[b]);
		shdr->SetMat3("norms[" + b_string + "]", norms[b]);
		shdr->SetMat4("b_axis[" + b_string + "]", axis[b]);
		shdr->SetMat4("b_axis_i[" + b_string + "]", axis_i[b]);
	}

	Mesh::Draw(shdr);
}

void SkelMesh::SetAnim(unsigned int id)
{
	if (id < skeleton->GetNumAnims())
		anim = id;

	anim_time = 0.0f;
	curFrame = 0;
	nexFrame = (skeleton->GetNumFrames(anim) > 1) ? 1 : 0;
}

void SkelMesh::SetFrame(unsigned int id)
{
	if(id < skeleton->GetNumFrames(anim))
	{ curFrame = id; }
}

void SkelMesh::NextFrame()
{
	curFrame = nexFrame;
	nexFrame = (nexFrame < skeleton->GetNumFrames(anim) -1) ? nexFrame + 1 : 0;
}

void SkelMesh::DrawSkeleton(glm::mat4 global, Shader* shdr)
{
	skeleton->DrawSkeleton(global, anim, curFrame, shdr);
}