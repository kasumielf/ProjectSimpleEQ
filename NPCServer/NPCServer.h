#pragma once

#include <unordered_map>
#include <deque>
#include "../GSLibrary/BaseServer.h"
#include "NonPlayer.h"

#include "tinyxml2/tinyxml2.h"
class NPCServer : public BaseServer
{
private:
	int last_add_npc_id;

	std::unordered_map<unsigned int, NonPlayer*> npcs;
	std::unordered_map<unsigned int, Object*> players;
	std::deque<NonPlayer*> regenQueue;

	void CreateNPCFromResource(const char* xmlfilename, unsigned short x, unsigned short y);

public:
	NPCServer(const int capacity, const short port);
	~NPCServer();

	void CreateNPC(NonPlayer *npc);
	void ClearNPCs();
	void ClearPlayers();
	void InitTemporaryNPCs();

	void NPCAttackUpdate(unsigned int id, NPCServer* self);

	virtual void ProcessPacket(const int id, unsigned char* packet);
	virtual void Logging(const wchar_t* msg, ...);
	virtual void OnCloseSocket(const int id) {}

	bool IsClosed(short from_x, short from_y, short to_x, short to_y);
};

