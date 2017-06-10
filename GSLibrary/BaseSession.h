#pragma once

#include <WinSock2.h>
#include "OverlappedEx.h"

class BaseSession
{
public:
	OverlappedEx overlapped;
	short id;
	bool use;
	unsigned char packet_buffer[MAX_PACKET_BUFFER_SIZE];
	int recvBytes;
	int received;
	int sendBytes;
	SOCKET socket;

public:
	BaseSession();
	~BaseSession();
};

