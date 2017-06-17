#include "AuthServer.h"
#include "../Common/ClientPacket.h"
#include "../Common/ServerPacket.h"
#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"

#include <iostream>

void AuthServer::ProcessPacket(const int id, unsigned char * packet)
{
	unsigned char packet_id = packet[0];

	Logging(L"Packet ID Is %d", packet_id);

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_LOGIN:
			{
				LOGIN *recvPacket = reinterpret_cast<LOGIN*>(packet);
				
				Request_Auth_To_DB_IsUserExist req;
				req.RESPONSE_ID = id;
				wcscpy_s(req.username, recvPacket->ID_STR);
				SendToInternal("DB", reinterpret_cast<unsigned char*>(&req));

				Logging(L"Login request by username %ws", recvPacket->ID_STR);
				break;
			}
			case ID_LOGOUT:
			{
				LOGOUT *recvPacket = reinterpret_cast<LOGOUT*>(packet);

				Logging(L"Logout request from %d", id);

				CloseSocket(id);

				break;
			}
			case ID_Response_DB_To_Auth_UserExist:
			{
				Response_DB_To_Auth_IsUserExist *recvPacket = reinterpret_cast<Response_DB_To_Auth_IsUserExist*>(packet);
				unsigned int uuid = recvPacket->user_uid;

				if (uuid > 0)
				{
					Request_Auth_To_World_AllocateUser req;

					req.RESPONSE_ID = recvPacket->RESPONSE_ID;
					req.user_uid = uuid;
					
					SendToInternal("World", reinterpret_cast<unsigned char*>(&req));
				}
				else
				{
					LOGIN_FAIL res;
					Send(recvPacket->client_id, reinterpret_cast<unsigned char*>(&res));
				}

				break;
			}
			case ID_Response_World_To_Auth_AllocateUser:
			{
				Response_World_To_Auth_AllocateUser *res = reinterpret_cast<Response_World_To_Auth_AllocateUser*>(packet);
				
				if (res->success)
				{
					CONNECT_SERVER connPacket;
					// ToDo : 하드 코딩된거 수정할 것.
					wcscpy_s(connPacket.ip, L"127.0.0.1");
					connPacket.port = 4003;
					connPacket.user_uid = res->user_uid;

					Logging(L"User is allocated to World. Send World Server Connect request");
					Send(res->client_id, reinterpret_cast<unsigned char*>(&connPacket));
				}
				else
				{
					LOGIN_FAIL not;
					Send(res->client_id, reinterpret_cast<unsigned char*>(&not));
					Logging(L"Login is failed. User is not exist");

				}
			}
		}
	}
}

void AuthServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}

void AuthServer::OnCloseSocket(const int id)
{
	Logging(L"Player %d is disconnected from Auth", id);
}
