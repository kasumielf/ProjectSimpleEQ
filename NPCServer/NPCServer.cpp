#include "NPCServer.h"
#include <iostream>

void NPCServer::ProcessPacket(const int id, char * packet)
{
}

void NPCServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}
