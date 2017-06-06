#include "Object.h"
#include <iostream>

Object::~Object()
{
}

void Object::SetName(wchar_t * _name)
{
	wcscpy_s(name, _name);
}
