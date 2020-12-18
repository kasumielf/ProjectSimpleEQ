#pragma once

#include <WinSock2.h>
#include "../Common/Defines.h"
#include "IOCPOpType.h"

struct OverlappedEx
{
    WSAOVERLAPPED    wsaOverlapped;
    WSABUF            wsaBuf;
    unsigned char    iocp_buffer[MAX_PACKET_BUFFER_SIZE];
    IOCPOpType        optype;
    int                caller_id;
};