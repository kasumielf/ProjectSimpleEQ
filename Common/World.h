#pragma once

#include <unordered_map>
#include <vector>
#include "../Common/Defines.h"
#include "Object.h"
#include "Player.h"
#include "NonPlayer.h"

class World
{
private:
	std::unordered_map<unsigned int, Object*> objects;
	Object* world[MAX_WORLD_HEIGHT][MAX_WORLD_WIDTH];

public:
	World();
	~World();

	Object* const GetObject(unsigned short x, unsigned short y) { return world[y][x]; }
	Object* const GetObjectById(unsigned int id);

	auto GetObjectsBegin() { return objects.begin(); }
	auto GetObjectsEnd() { return objects.end(); }

	void CreateObject(Object* obj);
	void DeleteObject(Object* obj);

	void AddObject(Object* obj, unsigned short x, unsigned short y) { world[y][x] = obj; }
	void RemoveObject(unsigned short x, unsigned short y);
	void MoveObject(unsigned short from_x, unsigned short from_y, unsigned short to_x, unsigned short to_y);
	
	void Destroy();
};

