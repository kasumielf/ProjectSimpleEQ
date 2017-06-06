#include "World.h"

World::World()
{
}


World::~World()
{
}

Object * const World::GetObjectById(unsigned int id)
{
	if (objects.count(id) > 0)
	{
		return objects[id];
	}

	return nullptr;
}

void World::CreateObject(Object * obj)
{
	unsigned int id = obj->GetId();

	if (objects[id] == nullptr)
	{
		objects[id] = obj;
	}
}

void World::DeleteObject(Object * obj)
{
	unsigned int id = obj->GetId();

	if (objects[id] != nullptr)
	{
		objects[id] = nullptr;
	}
}

void World::RemoveObject(unsigned short x, unsigned short y)
{
	world[y][x] = nullptr;
}

void World::MoveObject(unsigned short from_x, unsigned short from_y, unsigned short to_x, unsigned short to_y)
{
	if (world[to_y][to_x] == nullptr && world[from_y][from_x] != nullptr)
	{
		Object* ptr = world[from_y][from_x];
		world[from_y][from_x] = nullptr;
		world[to_y][to_x] = ptr;
	}
}

void World::Destroy()
{
}


