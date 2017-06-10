#pragma once

#include <unordered_map>
#include <deque>
#include "../GSLibrary/BaseServer.h"
#include "NonPlayer.h"

class NPCServer : public BaseServer
{
private:
	std::unordered_map<unsigned int, NonPlayer*> npcs;
	std::unordered_map<unsigned int, Object*> players;
	std::deque<NonPlayer*> regenQueue;

public:
	NPCServer(const int capacity, const short port) : BaseServer(capacity, port) {}
	~NPCServer();

	void ClearNPCs();
	void ClearPlayers();
	void InitTemporaryNPCs();

	virtual void ProcessPacket(const int id, unsigned char* packet);
	virtual void Logging(const wchar_t* msg, ...);
	virtual void OnCloseSocket(const int id) {}
};

