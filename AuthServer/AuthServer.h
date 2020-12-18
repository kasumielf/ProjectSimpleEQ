#pragma once

#include "../GSLibrary/BaseServer.h"

class AuthServer :
    public BaseServer
{
public:
    AuthServer(const int capacity, const short port) : BaseServer(capacity, port, false) {}
    ~AuthServer() {}

    virtual void ProcessPacket(const int id, unsigned char* packet);
    virtual void Logging(const wchar_t* msg, ...);
    virtual void OnCloseSocket(const int id);
};

