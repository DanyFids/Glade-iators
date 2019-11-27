#include "Skeleton.h"
#include "Object.h"
#include<fstream>
#include<iostream>

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

void Joint::FillJointArray(Transform* arr, int& cur, int anim = 0, int frame = 0)
{
	arr[cur] = animations[anim][frame];
	cur++;

	for (int c = 0; c < children.size(); c++) {
		children[c]->FillJointArray(arr, cur);
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
				cur->offset.y = std::stof(read);
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

Transform* Skeleton::GetTransformArray(int anim = 0, int frame = 0)
{
	Transform* ret = new Transform[num_bones];

	int id = 0;

	for (int c = 0; c < root->children.size(); c++) {
		root->children[c]->FillJointArray(ret, id, anim, frame);
	}

	return ret;
}
