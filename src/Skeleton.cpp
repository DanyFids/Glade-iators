#include <GLM/gtc/type_ptr.hpp>
#include "Skeleton.h"
#include "Object.h"
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include<fstream>
#include<iostream>
#include<sstream>
#include <math.h>
#include <stack>
#include <chrono>

ChannelType StringToChnlTp(std::string name)
{
	if (name.compare("Xposition") == 0) {
		return ChannelType::Xposition;
	}
	if (name.compare("Yposition") == 0) {
		return ChannelType::Yposition;
	}
	if (name.compare("Zposition") == 0) {
		return ChannelType::Zposition;
	}
	if (name.compare("Xrotation") == 0) {
		return ChannelType::Xrotation;
	}
	if (name.compare("Yrotation") == 0) {
		return ChannelType::Yrotation;
	}
	if (name.compare("Zrotation") == 0) {
		return ChannelType::Zrotation;
	}

	return ChannelType::INVALID_CHNL_TYPE;
}

glm::mat4 FindAxisRot(glm::vec3 tail) {
	glm::vec3 base(0.0f, 0.0f, 1.0f);
	glm::vec3 ntail = glm::normalize(tail);

	glm::vec3 rot = glm::vec3();

	rot.x = std::atan2f(-tail.y, tail.z);
	rot.y = std::atan2f(tail.x, tail.z);
	//rot.z = std::atan2f(ntail.x, ntail.y);

	glm::mat4 axisRot = glm::mat4(glm::angleAxis(rot.x, glm::vec3(1.0f, 0.0f, 0.0f)) * glm::angleAxis(rot.y, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::angleAxis(rot.z, glm::vec3(0.0f, 0.0f, 1.0f)));

	glm::vec4 test = glm::inverse(axisRot) * (glm::vec4(tail / glm::length(tail), 1.0f));
	glm::vec4 test2 = glm::vec4(glm::vec3(test.x, test.y, test.z) * glm::length(tail), 1.0) * axisRot;
	glm::vec4 test3 = axisRot * glm::vec4(glm::vec3(test.x, test.y, test.z) * glm::length(tail), 1.0);
	glm::vec4 test4 = glm::inverse(axisRot) * glm::rotate(glm::mat4(1.0f), 0.5f, {0.0f, 1.0f, 0.0f}) * axisRot * glm::vec4(tail, 1.0f);
	glm::vec4 test5 = glm::rotate(glm::mat4(1.0f), 0.5f, { 0.0f, 1.0f, 0.0f }) * glm::vec4(tail, 1.0f);


 	return axisRot;
}

Mesh* Joint::node_mesh = nullptr;
Material* Joint::node_mat = nullptr;

void Joint::init()
{
	if (node_mesh == nullptr) {
		node_mesh = new Mesh("node.obj");
	}
	if (node_mat == nullptr) node_mat = new Material("default-texture.png", "default-normal.png");
}

Joint::Joint(std::string n, Joint* p):name(n), parent(p)
{
	children = {};
	channels = {};

	Transform t;

	t.rotation = t.position = offset = glm::vec3(0.0f, 0.0f, 0.0f);

	animations = { { t } };
}

Joint* Joint::Find(std::string name)
{
	Joint* ret = nullptr;

	if (this->name.compare(name) == 0) {
		return this;
	}

	for (int c = 0; c < children.size(); c++) {
		ret = children[c]->Find(name);
		if (ret != nullptr)
			break;
	}

	return ret;
}

void Joint::WriteOutput(std::string pref)
{
	if (children.size() > 0) {
		std::cout << pref << (char)194 << name << std::endl;
		std::string temp = "";

		for (int s = 0; s < pref.length(); s++) {
			if (pref[s] == (char)195) {
				temp = temp + (char)179;
			}
			else if (pref[s] == (char)192) {
				temp = temp + " ";
			}
			else {
				temp = temp + pref[s];
			}

		}

		for (int c = 0; c < children.size(); c++) {
			if (c != children.size() - 1)
				children[c]->WriteOutput(temp + (char)195);
			else
				children[c]->WriteOutput(temp + (char)192);
		}
	}
	else {
		std::cout << pref << (char)196 << name << std::endl;
	}
}

void Joint::WriteTransform(int anim, int frame)
{
	std::cout << name << std::endl;
	std::cout << "Offset:   " + std::to_string(offset.x) + " " + std::to_string(offset.y) + " " + std::to_string(offset.z) << std::endl;
	std::cout << "Position: " + std::to_string(animations[anim][frame].position.x) + " " + std::to_string(animations[anim][frame].position.y) + " " + std::to_string(animations[anim][frame].position.z) << std::endl;
	std::cout << "Rotation: " + std::to_string(animations[anim][frame].rotation.x) + " " + std::to_string(animations[anim][frame].rotation.y) + " " + std::to_string(animations[anim][frame].rotation.z) << std::endl << std::endl;
}

void Joint::Draw(glm::mat4 global, int a, int f, Shader* shader)
{
	shader->Use();
	shader->SetI("material.diffuse", 0);
	shader->SetI("material.normal", 1);
	shader->SetI("material.specular", 2);

	glm::mat4 model = global * animations[a][f].GetWorldTransform();

	unsigned int modelLoc = glGetUniformLocation(shader->ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glm::mat3 normMat = glm::mat3(glm::transpose(glm::inverse(model)));

	shader->SetMat3("normMat", normMat);

	shader->SetF("material.shine", node_mat->shine);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, node_mat->DIFF);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, node_mat->NORM);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, node_mat->SPEC);

	node_mesh->Draw(shader);

	for (int c = 0; c < children.size(); c++) {
		children[c]->Draw(model, a, f, shader);
	}
}

void Joint::FillJointArray(glm::mat4* arr, glm::mat4*& axis, glm::mat4*& axis_i, glm::mat3* norms, glm::mat4 global, glm::vec3* binds, glm::vec3 last, glm::vec3*& bind_t, glm::vec3 last_b, float* anim_i, int* anim_c, int& cur, int* frame, int num_channels)
{
	Transform local_trans;

	for (int c = 0; c < num_channels; c++) {
		if (anim_c[c] >= 0 && anim_c[c] < animations.size()) {
			local_trans.position += (animations[anim_c[c]][frame[c]].position - offset) * anim_i[c];
			local_trans.rotation += animations[anim_c[c]][frame[c]].rotation * anim_i[c];
			local_trans.scale += (animations[anim_c[c]][frame[c]].scale - glm::vec3(1.0f)) * anim_i[c];
		}
	}

	local_trans.position += offset;

	//glm::mat4 trans = global * local_trans;
	glm::mat4 trans = global * local_trans.GetWorldTransform();
	glm::vec3 bind = last + offset;
	glm::mat3 norm = glm::mat3(glm::transpose(glm::inverse(trans)));
	arr[cur] = trans;
	binds[cur] = bind;
	axis[cur] = axisRot;
	axis_i[cur] = glm::inverse(axisRot);
	//bind_t[cur] = global;
	norms[cur] = norm;
	cur++;
	
	/*if ((name.compare("l_shoulder") == 0 || name.compare("r_shoulder") == 0 || name.compare("neck") == 0 ) && frame == 1) {
		std::cout << name << std::endl;
		std::cout << "rotation:   " << animations[anim][frame].rotation.x << " " << animations[anim][frame].rotation.y << " " << animations[anim][frame].rotation.z << std::endl;
		std::cout << std::endl;
	}*/

	for (int c = 0; c < children.size(); c++) {
		glm::vec3 next_bt = last_b + glm::vec3(trans * glm::vec4(children[c]->offset, 0.0f));
		bind_t[cur] = next_bt;
		children[c]->FillJointArray(arr, axis, axis_i, norms, trans, binds, bind, bind_t, next_bt, anim_i, anim_c, cur, frame, num_channels);
	}
}

void Joint::FillFrameRot(glm::mat4* arr, glm::mat3* norms, glm::mat4 global, int& cur, int anim, int frame)
{
	glm::mat4 trans = global * animations[anim][frame].GetRotEul();
	glm::mat3 norm = glm::mat3(glm::transpose(glm::inverse(trans)));

	arr[cur] = trans;
	norms[cur] = norm;
	cur++;

	for (int c = 0; c < children.size(); c++) {
		children[c]->FillFrameRot(arr, norms, trans, cur, anim, frame);
	}
}

void Joint::LoadAnimFrame(std::queue<float>& values, int anim, int frame)
{
	if (animations.size() <= anim) {
		animations.push_back(std::vector<Transform>());
	}

	if (animations[anim].size() <= frame) {
		animations[anim].push_back(Transform());
	}

	for (int c = 0; c < channels.size(); c++) {
		float val = values.front();

		float corr = 1.0f;

		switch (channels[c]) {
		case ChannelType::Xposition:
			animations[anim][frame].position.x = val;
			break;
		case ChannelType::Yposition:
			animations[anim][frame].position.y = val;
			break;
		case ChannelType::Zposition:
			animations[anim][frame].position.z = val;
			break;
		case ChannelType::Xrotation:
			animations[anim][frame].rotation.x = val;
			break;
		case ChannelType::Yrotation:
			animations[anim][frame].rotation.y = val;
			break;
		case ChannelType::Zrotation:
			animations[anim][frame].rotation.z = val;
			break;
		default:
			std::cout << "ERROR::SKELETON:: '" + name + "' ::LOAD_FRAME::INVALID_CHANNEL";
			break;
		}

		values.pop();
	}

	if (channels.size() <= 3 && (int)channels[0] > 3) {
		animations[anim][frame].position = offset;
	}

	//animations[anim][frame].position = glm::vec3(0.0f, 0.0f, glm::length(offset));

	for (int c = 0; c < children.size(); c++) {
		children[c]->LoadAnimFrame(values, anim, frame);
	}
}

glm::mat4 Joint::TransformTo(int anim, int frame)
{
	if (parent != nullptr) 
		return parent->TransformTo(anim, frame) * animations[anim][frame].GetWorldTransform();
	else {
		return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, animations[anim][frame].position.y, 0.0f)) * animations[anim][frame].GetRotEul();
	}
}

glm::mat4 Joint::TransformTo(int* anims, int* frames, float* intensities, unsigned int num_chnls)
{
	Transform temp;
	for (int c = 0; c < num_chnls; c++) {
		if (anims[c] >= 0 && anims[c] < animations.size()) {
			temp.position += (animations[anims[c]][frames[c]].position - offset) * intensities[c];
			temp.rotation += animations[anims[c]][frames[c]].rotation * intensities[c];
			//temp.scale += animations[anims[c]][frames[c]].scale * intensities[c];
		}
	}

	temp.position += offset;

	if (parent != nullptr) {
		return parent->TransformTo(anims, frames, intensities, num_chnls) * temp.GetWorldTransform();
	}
	else {
		return glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, temp.position.y, 0.0f)) * temp.GetRotEul();
	}
}

Skeleton::Skeleton(std::string name, std::string file)
{
	this->name = name;

	LoadFromFile(file);
}

void Skeleton::WriteTree()
{
	root->WriteOutput("");

	std::cout << std::endl;
	for (int c = 0; c < anim_names.size(); c++) {
		std::cout << anim_names[c] << std::endl;
	}
	std::cout << std::endl;
}

int Skeleton::LoadFromFile(std::string f)
{
	std::ifstream file;
	file.open(f, std::ios::in);

	if (!file) return -1;
	if (!file.good()) return -2;

	std::string read;
	Joint* cur = nullptr;

	num_bones = 0;

	while (!file.eof()) {
		if (file.peek() == '#') {
			file.ignore(512, (int)'\n');
			continue;
		}
		file >> read;

		if (read.compare("ROOT") == 0) {
			file >> read;
			root = new Joint(read, nullptr);
			cur = root;
			//num_bones++;
			continue;
		}
		else if (read.compare("JOINT") == 0) {
			file >> read;
			if (cur != nullptr) {
				Joint* next = new Joint(read, cur);
				cur->children.push_back(next);
				cur = next;
				num_bones++;
				continue;
			}
			else {
				return -3;
			}
		}
		else if (read.compare("OFFSET") == 0) {
			if (cur != nullptr) {
				file >> read;
				cur->offset.x = std::stof(read);
				file >> read;
				cur->offset.y = std::stof(read);
				file >> read;
				cur->offset.z = std::stof(read);

				cur->animations[0][0].position = cur->offset;
				continue;
			}
			else return -3;
		}
		else if (read.compare("CHANNELS") == 0) {
			file >> read;
			int numC = std::stoi(read);

			for (int r = 0; r < numC; r++) {
				file >> read;
				cur->channels.push_back(StringToChnlTp(read));
			}
		}
		else if (read.compare("}") == 0) {
			if (cur != nullptr) {
				if (cur->children.size() > 0) {
					glm::vec3 avg = glm::vec3(0.0f);
					for (int c = 0; c < cur->children.size(); c++) {
						avg += cur->children[c]->offset;
					}
					avg /= cur->children.size();

					cur->tail = avg;
				}


				cur->axisRot = FindAxisRot(cur->tail);

				cur = cur->parent;
				continue;
			}
			else return -3;
		}
		else if (read.compare("End") == 0) {
			glm::vec3 temp;

			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');

			file >> read; // eat OFFSET
			file >> read; // x offset;
			temp.x = std::stof(read);
			
			file >> read; // y offset
			temp.z = std::stof(read);

			file >> read; // z offset
			temp.y = std::stof(read);

			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');

			cur->tail = temp;

			continue;
		}
		else if (read.compare("MOTION") == 0) {
			file >> read;
			
			anim_names.push_back(read);

			int num_frames = 0;
			int anim = root->animations.size();

			AnimStates.push_back(std::vector<FrameStates>());

			file >> read; // eat
			file >> read;
			
			int expected_frames = std::stoi(read);

			file >> read; // eat
			file >> read; // eat
			file >> read;

			anim_ft.push_back(std::stof(read));

			file.ignore(512, '\n');

			int yeetval = 1;
			int yootval = 1;
			while (!file.eof()) {
				auto start_time = std::chrono::high_resolution_clock().now();

				std::queue<float> values;
				std::stringstream line;
				std::string temp;

				std::getline(file, temp);

				if (temp.compare("") == 0) {
					break;
				}

				line.str(temp);

				if (isalpha(line.peek())) {
					line >> read;

					if (read.compare("N") == 0)
					{
						AnimStates[AnimStates.size() - 1].push_back(Neutral);
					} 
					else if (read.compare("E") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(End);
					}
					else if (read.compare("R") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(Roll);
					}
					else if (read.compare("D") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(Deflect);
					}
					else if (read.compare("A") == 0)
					{
						AnimStates[AnimStates.size() - 1].push_back(Attack);
					}
					else if (read.compare("B") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(Block);
					}
					else if (read.compare("H") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(Hold);
					}
					else if (read.compare("T") == 0) {
						AnimStates[AnimStates.size() - 1].push_back(Taunt);
					}
				}
				else {
					AnimStates[AnimStates.size() - 1].push_back(Neutral);
				}

				while (!line.eof()) {
					line >> read;
					values.push(std::stof(read));
				}
				root->LoadAnimFrame(values, anim, num_frames);
				num_frames++;

				auto end_time = std::chrono::high_resolution_clock().now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

				std::cout << duration.count() << std::endl;
			}
		}
		else {
			file.ignore(512, (int)'\n');
			continue;
		}
	}

	return 0;
}

Joint* Skeleton::Find(std::string name)
{
	Joint* ret = nullptr;
	if (root != nullptr) {
		ret = root->Find(name);
	}

	return ret;
}

void Skeleton::GetTransformArray(glm::mat4* & ret, glm::mat4* & axis, glm::mat4* & axis_i, glm::mat3* & norms, glm::vec3* & binds, glm::vec3*& bind_t, float* anim_i, int* anim_c, int* frame, int num_channels)
{
	ret = new glm::mat4[num_bones];
	binds = new glm::vec3[num_bones];
	bind_t = new glm::vec3[num_bones];
	norms = new glm::mat3[num_bones];
	axis = new glm::mat4[num_bones];
	axis_i = new glm::mat4[num_bones];

	int id = 0;

	Transform tmp;

	for (int c = 0; c < num_channels; c++) {
		if (anim_c[c] >= 0 && anim_c[c] < root->animations.size()) {
			tmp.position += glm::vec3(0.0f, root->animations[anim_c[c]][frame[c]].position.y - root->offset.y, 0.0f) * anim_i[c];
			tmp.rotation += root->animations[anim_c[c]][frame[c]].rotation * anim_i[c];
			//tmp.scale += root->animations[anim_c[c]][frame[c]].scale * anim_i[c];
		}
	}

	tmp.position.y += root->offset.y;

	glm::mat4 r_tran = tmp.GetWorldTransform();
	for (int c = 0; c < root->children.size(); c++) {
		glm::vec3 newPos = root->offset + glm::vec3(r_tran * glm::vec4(root->children[c]->offset, 0.0f));
		bind_t[id] = glm::vec3(newPos);
		root->children[c]->FillJointArray(ret, axis, axis_i, norms, r_tran, binds, root->offset, bind_t, newPos, anim_i, anim_c, id, frame, num_channels); // float* anim_i, int* anim_c, int& cur, int* frame, int num_channels
	}

}

void Skeleton::GetFrameRot(glm::mat4*& bones, glm::mat3*& norms, int anim, int frame)
{
	bones = new glm::mat4[num_bones];
	norms = new glm::mat3[num_bones];

	int id = 0;

	for (int c = 0; c < root->children.size(); c++) {
		root->children[c]->FillFrameRot(bones, norms, root->animations[anim][frame].GetRotEul(), id, anim, frame);
	}
}

int Skeleton::GetAnimByName(std::string n)
{
	int ret;

	for (ret = 0; ret < anim_names.size(); ret++) {
		if (anim_names[ret].compare(n) == 0)
			break;
	}

	if (ret < anim_names.size())
		return ret;
	else
		return -1;
}
