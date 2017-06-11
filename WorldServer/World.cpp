#include "World.h"

World::World()
{
	for (int i = 0; i < MAX_WORLD_HEIGHT; ++i)
	{
		for (int j = 0; j < MAX_WORLD_WIDTH; ++j)
		{
			world[i][j] = nullptr;
		}
	}
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

void World::DeleteObject(unsigned int id)
{
	if (objects[id] != nullptr)
	{
		DeleteObject(objects[id]);
	}
}

void World::DeleteObject(Object * obj)
{
	sector[obj->getCurrSectorY()][obj->getCurrSectorX()].RemovePlayer(obj->GetId());
	objects.erase(obj->GetId());
	RemoveObject(obj->GetX(), obj->GetY());
	delete obj;
	obj = nullptr;
}

void World::RemoveObject(unsigned short x, unsigned short y)
{
	world[y][x] = nullptr;
}

void World::MoveObject(Object * obj, const char direction)
{
	short from_x = obj->GetX(), from_y = obj->GetY();
	obj->MoveByDirection(direction);
	short to_x = obj->GetX(), to_y = obj->GetY();

	if (world[to_y][to_x] == nullptr && world[from_y][from_x] != nullptr)
	{
		Object* ptr = world[from_y][from_x];
		world[from_y][from_x] = nullptr;
		world[to_y][to_x] = ptr;
	}

	SetSector(obj, to_x, to_y);
}

void World::SetSector(Object* obj, unsigned short x, unsigned short y)
{
	if (sector[y / MAX_SECTOR_HEIGHT][x / MAX_SECTOR_WIDTH].GetPlayer(obj->GetId()) == nullptr)
	{
		if (obj->getCurrSectorX() >= 0)
			sector[obj->getCurrSectorY()][obj->getCurrSectorX()].RemovePlayer(obj->GetId());

		sector[y / MAX_SECTOR_HEIGHT][x / MAX_SECTOR_WIDTH].AddPlayer(obj->GetId(), obj);
		obj->setCurSectorPos(x / MAX_SECTOR_HEIGHT, y / MAX_SECTOR_WIDTH);
	}

}
void World::Destroy()
{
}


