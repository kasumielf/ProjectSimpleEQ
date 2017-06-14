#pragma once

#include "../GSLibrary/BaseServer.h"
#include "World.h"
#include "Status.h"

#include <vector>


class WorldServer :
	public BaseServer
{
private:
	World *world;
	std::vector<Player*> players;
	std::unordered_map<unsigned int, unsigned int> socketIds;
	std::vector<Status> level_data;

public:
	WorldServer(const int capacity, const short port);
	~WorldServer();

	virtual void ProcessPacket(const int id, unsigned char* packet);
	virtual void Logging(const wchar_t* msg, ...);
	virtual void OnCloseSocket(const int id);

	World* const GetWorldPtr() { return world; }
	
	void PlayerAttackUpdate(unsigned int id, WorldServer* self);
	void PlayerUpdate(unsigned int id, WorldServer* self);
	void PlayerDBSave(unsigned int id, WorldServer * self);

	bool IsClosed(short from_x, short from_y, short to_x, short to_y);
	Status& GetLevelData(unsigned int level) { return level_data[level]; }

	void InitStatusTable();
	void MovePlayer(int id, Player* p);

};

