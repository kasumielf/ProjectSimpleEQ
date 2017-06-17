#include "Object.h"
#include "../Common/Defines.h"
#include <iostream>

Object::~Object()
{
}

void Object::SetName(wchar_t * _name)
{
	wcscpy_s(name, _name);
}

void Object::MoveByDirection(const char direction)
{
	switch (direction)
	{
	case 0:	y++; break;
	case 2: x++; break;
	case 4: y--; break;
	case 6: x--; break;
	}

	if (x > MAX_WORLD_WIDTH)
		x = MAX_WORLD_WIDTH - 1;

	if (x < 0)
		x = 0;

	if (y > MAX_WORLD_HEIGHT)
		y = MAX_WORLD_HEIGHT - 1;

	if (y < 0)
		y = 0;

}
