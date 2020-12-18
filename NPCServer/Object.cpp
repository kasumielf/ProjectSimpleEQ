#include "Object.h"
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
    case 2:    y++; break;
    case 4: x++; break;
    case 6: y--; break;
    case 8: x--; break;
    }
}
