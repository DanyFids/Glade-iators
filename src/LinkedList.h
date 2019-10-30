#pragma once

template<class T>
class Link {
private:
	T value;
	Link* next = nullptr;
public:
	Link(T val) { value = val; }
	T GetValue() { return value; }
	Link* GetNext() { return next; }
	void SetNext(Link* n) { next = n; }
};

template<class T>
class LinkedList {
	Link* start = nullptr;
	Link* cur = nullptr;
	int size = 0;
public:
	LinkedList() {};

	int Size() { return size; }
	void Add(T val) {
		if (size <= 0) {
			start = new Link(val);
		}
		else {
			cur = start;
			for (int c = 0; c < size; c++) {
				cur = cur->GetNext();
			}
			cur->SetNext(new Link(val));
		}
		size++;
	};

	Link* At(int id) {
		if (id < size) {
			cur = start;
			for (int c = 0; c < id; c++) {
				cur = cur->GetNext();
			}
			return cur;
		}
	}
};

class Object;
LinkedList<Object*> myLinkedList;

class Player : public LinkedList<Object*> {

};