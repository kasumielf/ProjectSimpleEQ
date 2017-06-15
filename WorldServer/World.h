#pragma once

#include <unordered_map>
#include <vector>

#include "Object.h"
#include "Player.h"
#include "NonPlayer.h"
#include "Sector.h"

#include "../Common/Defines.h"

class World
{
private:
	std::unordered_map<unsigned int, Object*> objects;
	Object* world[MAX_WORLD_HEIGHT][MAX_WORLD_WIDTH];
	Sector sector[MAX_WORLD_HEIGHT / MAX_SECTOR_HEIGHT][MAX_WORLD_WIDTH / MAX_SECTOR_WIDTH];
public:
	World();
	~World();

	Object* const GetObject(unsigned short x, unsigned short y) { return world[y][x]; }
	Object* const GetObjectById(unsigned int id);

	void WorldInit();

	auto GetPlayerBegin(const short sector_x, const short sector_y) { return sector[sector_y][sector_x].begin(); }
	auto GetPlayerEnd(const short sector_x, const short sector_y) { return sector[sector_y][sector_x].end(); }

	void CreateObject(Object* obj);
	void DeleteObject(unsigned int id);
	void DeleteObject(Object* obj);

	void AddObject(Object* obj, unsigned short x, unsigned short y) { world[y][x] = obj; }
	void RemoveObject(unsigned short x, unsigned short y);
	void MoveObject(Object* obj, const char direction);
	void SetSector(Object* obj, unsigned short x, unsigned short y);

	void Destroy();
};

