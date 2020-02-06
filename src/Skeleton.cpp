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

	if (tail.x != 0 && tail.y != 0 && tail.z != 0) {
		std::cout << "HOI!!!" << std::endl;
	}

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

void Joint::FillJointArray(glm::mat4* arr, glm::mat4*& axis, glm::mat4*& axis_i, glm::mat3* norms, glm::mat4 global, glm::vec3* binds, glm::vec3 last, glm::vec3*& bind_t, glm::vec3 last_b, int& cur, int anim = 0, int frame = 0)
{
	glm::mat4 local_trans;

	if (parent != nullptr) {
		std::stack<glm::mat4> parentStack;
		Joint* cur = parent;
		while (cur != nullptr) {
			parentStack.push(cur->axisRot);
			cur = cur->parent;
		}

		glm::mat4 local_rot = animations[anim][frame].GetRotEul();

		while (parentStack.size() > 0) {
			auto par = parentStack.top();
			local_rot = glm::inverse(par) * local_rot * par;
			parentStack.pop();
		}

		local_rot = local_rot * axisRot;

		local_trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 1.0f) * glm::length(offset)) * // Offset
			glm::translate(glm::mat4(1.0f), animations[anim][frame].position - offset) * // Translations
			local_rot * // Rotation
			glm::scale(glm::mat4(1.0f), animations[anim][frame].scale); // scale
	}
	else {
		local_trans = glm::translate(glm::mat4(1.0f), animations[anim][frame].position) * axisRot * animations[anim][frame].GetRotEul() * glm::scale(glm::mat4(1.0f), animations[anim][frame].scale);
	}

	//glm::mat4 trans = global * local_trans;
	glm::mat4 trans = global * animations[anim][frame].GetWorldTransform();
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
		children[c]->FillJointArray(arr, axis, axis_i, norms, trans, binds, bind, bind_t, next_bt, cur, anim, frame);
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

	if (this->name.compare("l_leg2") == 0) {
		std::cout << "HOI!!" << std::endl;
	}

	for (int c = 0; c < channels.size(); c++) {
		float val = values.front();

		float corr = 1.0f;

		switch (channels[c]) {
		case ChannelType::Xposition:
			animations[anim][frame].position.x = val;
			break;
		case ChannelType::Yposition:
			animations[anim][frame].position.z = val;
			break;
		case ChannelType::Zposition:
			animations[anim][frame].position.y = val;
			break;
		case ChannelType::Xrotation:
			animations[anim][frame].rotation.x = -val;
			break;
		case ChannelType::Yrotation:
			animations[anim][frame].rotation.z = -val;
			break;
		case ChannelType::Zrotation:
			animations[anim][frame].rotation.y = val;
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

	if (name.compare("l_arm1") == 0 || name.compare("r_arm1") == 0) {
		std::cout << "HOI!!!" << std::endl;
	}

	for (int c = 0; c < children.size(); c++) {
		children[c]->LoadAnimFrame(values, anim, frame);
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
				cur->offset.x = -std::stof(read);
				file >> read;
				cur->offset.z = std::stof(read);
				file >> read;
				cur->offset.y = std::stof(read);

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

			file >> read; // eat
			file >> read;
			
			int expected_frames = std::stoi(read);

			file >> read; // eat
			file >> read; // eat
			file >> read;

			anim_ft.push_back(std::stof(read));

			file.ignore(512, '\n');

			while (!file.eof()) {
				std::queue<float> values;

				std::stringstream line;
				std::string temp;


				std::getline(file, temp);

				if (temp.compare("") == 0) {
					break;
				}

				line.str(temp);

				while (!line.eof()) {
					line >> read;
					values.push(std::stof(read));
				}
				root->LoadAnimFrame(values, anim, num_frames);
				num_frames++;
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

void Skeleton::GetTransformArray(glm::mat4* & ret, glm::mat4* & axis, glm::mat4* & axis_i, glm::mat3* & norms, glm::vec3* & binds, glm::vec3*& bind_t, int anim = 0, int frame = 0)
{
	ret = new glm::mat4[num_bones];
	binds = new glm::vec3[num_bones];
	bind_t = new glm::vec3[num_bones];
	norms = new glm::mat3[num_bones];
	axis = new glm::mat4[num_bones];
	axis_i = new glm::mat4[num_bones];

	int id = 0;

	for (int c = 0; c < root->children.size(); c++) {
		glm::vec3 newPos = root->offset + glm::vec3(root->animations[anim][frame].GetWorldTransform() * glm::vec4(root->children[c]->offset, 0.0f));
		bind_t[id] = glm::vec3(newPos);
		root->children[c]->FillJointArray(ret, axis, axis_i, norms, root->animations[anim][frame].GetWorldTransform(), binds, root->offset, bind_t, newPos, id, anim, frame);
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
