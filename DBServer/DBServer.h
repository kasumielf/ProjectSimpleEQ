#pragma once

#include "../GSLibrary/BaseServer.h"
#include "DBManager.h"


class DBServer :
	public BaseServer
{
private:
	DBManager m_db;

public:
	DBServer(const int capacity, const short port) : BaseServer(capacity, port, false) {}
	~DBServer() {}

	virtual void ProcessPacket(const int id, unsigned char* packet);
	virtual void Logging(const wchar_t* msg, ...);
	virtual void OnCloseSocket(const int id) {}

	void DBConnect(const wchar_t* ip, const wchar_t* scheme, const wchar_t* id, const wchar_t* pwd);
};

