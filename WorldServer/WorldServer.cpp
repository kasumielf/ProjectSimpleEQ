#include <iostream>

#include "WorldServer.h"
#include "../Common/ClientPacket.h"
#include "../Common/ServerPacket.h"
#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"

WorldServer::WorldServer(const int capacity, const short port) : BaseServer(capacity, port)
{
	world = new World();
}

WorldServer::~WorldServer()
{
	world->Destroy();
	world = nullptr;
	delete world;
}

void WorldServer::ProcessPacket(const int id, char * packet)
{
	unsigned char packet_id = packet[0];

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_Request_Auth_To_World_AllocateUser:
			{
				Request_Auth_To_World_AllocateUser *req = reinterpret_cast<Request_Auth_To_World_AllocateUser*>(packet);

				if (world->GetObjectById(req->user_uid) == nullptr)
				{
					Request_World_To_DB_GetUserStatus user_req;
					user_req.user_uid = req->user_uid;
					user_req.RESPONSE_ID = req->RESPONSE_ID;

					SendToInternal("DB", reinterpret_cast<char*>(&user_req));
				}
				else
				{
					Response_World_To_Auth_AllocateUser res;
					res.RESPONSE_ID = req->RESPONSE_ID;
					res.success = false;
					Send(id, reinterpret_cast<char*>(&res));
				}
				break;
			}

			case ID_Response_DB_To_World_GetUserStatus:
			{
				Response_DB_To_World_GetUserStatus *res = reinterpret_cast<Response_DB_To_World_GetUserStatus*>(packet);
				Response_World_To_Auth_AllocateUser auth_res;

				auth_res.success = res->user_uid > 0;

				if (auth_res.success)
				{
					Player *p = new Player(res->user_uid);
					p->SetName(res->username);
					p->SetX(res->x);
					p->SetY(res->y);
					p->SetHP(res->hp);
					p->SetLevel(res->level);
					p->SetExp(res->exp);

					world->CreateObject(p);
					world->AddObject(p, p->GetX(), p->GetY());
				}

				auth_res.RESPONSE_ID = res->RESPONSE_ID;
				Send(id, reinterpret_cast<char*>(&auth_res));

				break;
			}
		}
	}
}

void WorldServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}
