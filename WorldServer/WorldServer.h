#pragma once

#include "../GSLibrary/BaseServer.h"
#include "../Common/World.h"

class WorldServer :
	public BaseServer
{
private:
	World *world;

public:
	WorldServer(const int capacity, const short port);
	~WorldServer();

	virtual void ProcessPacket(const int id, char* packet);
	virtual void Logging(const wchar_t* msg, ...);

	World* const GetWorldPtr() { return world; }
};

