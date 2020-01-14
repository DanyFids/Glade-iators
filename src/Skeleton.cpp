#include "Skeleton.h"
#include "Object.h"
#include<fstream>
#include<iostream>
#include<sstream>

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

void Joint::FillJointArray(glm::mat4* arr, glm::mat4 global, glm::vec3* binds, glm::vec3 last, glm::vec3*& bind_t, glm::vec3 last_b, int& cur, int anim = 0, int frame = 0)
{
	glm::mat4 trans = global * animations[anim][frame].GetWorldTransform();
	glm::vec3 bind = last + offset;
	arr[cur] = trans;
	binds[cur] = bind;
	//bind_t[cur] = global;
	cur++;

	if (name.compare("l_shoulder") == 0 || name.compare("r_shoulder") == 0 || name.compare("neck") == 0 ) {
		std::cout << name << std::endl;
		std::cout << "global: " << global[3].x << " " << global[3].y << " " << global[3].z << std::endl;
		std::cout << "trans:  " << trans[3].x << " " << trans[3].y << " " << trans[3].z << std::endl;
		std::cout << "bind:   " << bind.x << " " << bind.y << " " << bind.z << std::endl;
		std::cout << "anim:   " << animations[anim][frame].position.x << " " << animations[anim][frame].position.y << " " << animations[anim][frame].position.z << std::endl;
		std::cout << std::endl;
	}

	for (int c = 0; c < children.size(); c++) {
		glm::vec3 next_bt = last_b + glm::vec3(trans * glm::vec4(children[c]->offset, 0.0f));
		bind_t[cur] = next_bt;
		children[c]->FillJointArray(arr, trans, binds, bind, bind_t, next_bt, cur, anim, frame);
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

		switch (channels[c]) {
		case ChannelType::Xposition:
			animations[anim][frame].position.x = val;
			break;
		case ChannelType::Yposition:
			animations[anim][frame].position.y = -val;
			break;
		case ChannelType::Zposition:
			animations[anim][frame].position.z = val;
			break;
		case ChannelType::Xrotation:
			animations[anim][frame].rotation.x = val;
			break;
		case ChannelType::Yrotation:
			animations[anim][frame].rotation.y = -val;
			break;
		case ChannelType::Zrotation:
			animations[anim][frame].rotation.z = -val;
			break;
		default:
			std::cout << "ERROR::SKELETON:: '" + name + "' ::LOAD_FRAME::INVALID_CHANNEL";
			break;
		}

		values.pop();
	}

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
				cur->offset.x = std::stof(read);
				file >> read;
				cur->offset.y = -std::stof(read);
				file >> read;
				cur->offset.z = std::stof(read);
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
				cur = cur->parent;
				continue;
			}
			else return -3;
		}
		else if (read.compare("End") == 0) {
			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');
			continue;
		}
		else if (read.compare("MOTION") == 0) {
			int num_frames = 0;
			int anim = root->animations.size();

			file >> read;
			file.ignore(512, (int)'\n');
			file.ignore(512, (int)'\n');

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

void Skeleton::GetTransformArray(glm::mat4* & ret, glm::vec3* & binds, glm::vec3*& bind_t, int anim = 0, int frame = 0)
{
	ret = new glm::mat4[num_bones];
	binds = new glm::vec3[num_bones];
	bind_t = new glm::vec3[num_bones];

	int id = 0;

	for (int c = 0; c < root->children.size(); c++) {
		glm::vec3 newPos = root->offset + glm::vec3(root->animations[anim][frame].GetWorldTransform() * glm::vec4(root->children[c]->offset, 0.0f));
		bind_t[id] = glm::vec3(newPos);
		root->children[c]->FillJointArray(ret, root->animations[anim][frame].GetWorldTransform(), binds, root->offset, bind_t, newPos, id, anim, frame);
	}

}
