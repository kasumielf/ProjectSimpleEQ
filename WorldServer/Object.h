#pragma once

#include "ObjectType.h"
#include "ObjectState.h"

class Object
{
private:
	wchar_t name[12];

	unsigned int id;
	short x;
	short y;

	ObjectType type;
	ObjectState state;

	short current_sector_x;
	short current_sector_y;

public:
	Object(unsigned int _id, ObjectType _type) : id(_id), type(_type), state(ObjectState::Idle) , current_sector_x(-1), current_sector_y(-1) {}
	~Object();

	void SetX(short _x) { x = _x; }
	void SetY(short _y) { y = _y; }
	void SetCurrentState(ObjectState _state) { state = _state; }
	void SetName(wchar_t* _name);

	const unsigned int GetId() { return id; }
	const ObjectType GetType() { return type; }
	const ObjectState GetCurrentState() { return state; }
	const short GetX() { return x; }
	const short GetY() { return y; }
	wchar_t* GetName() { return name; }

	void MoveByDirection(const char direction);
	void setCurSectorPos(const short x, const short y)
	{
		current_sector_x = x;
		current_sector_y = y;
	}
	const short getCurrSectorX() { return current_sector_x; }
	const short getCurrSectorY() { return current_sector_y; }

	virtual void Update() {}
};

