#pragma once

#include "../GSLibrary/BaseServer.h"

class NPCServer :
	public BaseServer
{
public:
	NPCServer(const int capacity, const short port) : BaseServer(capacity, port) {}
	~NPCServer() {}

	virtual void ProcessPacket(const int id, char* packet);
	virtual void Logging(const wchar_t* msg, ...);
};

