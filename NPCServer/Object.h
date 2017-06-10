#pragma once

#include "ObjectType.h"
#include "ObjectState.h"

class Object
{
private:
	wchar_t name[12];

	unsigned int id;
	unsigned int x;
	unsigned int y;

	ObjectType type;
	ObjectState state;

public:
	Object(unsigned int _id, ObjectType _type) : id(_id), type(_type), state(ObjectState::Idle){}
	~Object();

	void SetX(unsigned int _x) { x = _x; }
	void SetY(unsigned int _y) { y = _y; }
	void SetCurrentState(ObjectState _state) { state = _state; }
	void SetName(wchar_t* _name);

	const unsigned int GetId() { return id; }
	const ObjectType GetType() { return type; }
	const ObjectState GetCurrentState() { return state; }
	const unsigned int GetX() { return x; }
	const unsigned int GetY() { return y; }
	const wchar_t* const GetName() { return name; }

	void MoveByDirection(const char direction);

	virtual void Update() {}
};

