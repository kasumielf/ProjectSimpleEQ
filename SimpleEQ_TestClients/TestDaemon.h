#pragma once
#include "../GSLibrary/BaseServer.h"

class TestDaemon :
	public BaseServer
{
public:
	TestDaemon(const wchar_t* username) : BaseServer(1, 9999) {}
	~TestDaemon() {}

	virtual void ProcessPacket(const int id, unsigned char* packet) {};
	virtual void Logging(const wchar_t* msg, ...) {};
	virtual void OnCloseSocket(const int id) {}
};

