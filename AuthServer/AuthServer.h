#pragma once

#include "../GSLibrary/BaseServer.h"

class AuthServer :
	public BaseServer
{
public:
	AuthServer(const int capacity, const short port) : BaseServer(capacity, port) {}
	~AuthServer() {}

	virtual void ProcessPacket(const int id, char* packet);
	virtual void Logging(const wchar_t* msg, ...);
};

