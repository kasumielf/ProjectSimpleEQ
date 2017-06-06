#include "DBServer.h"
#include <iostream>

#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"

void DBServer::ProcessPacket(const int id, char * packet)
{
	unsigned char packet_id = packet[0];

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_Request_Auth_To_DB_IsUserExist:
			{
				Request_Auth_To_DB_IsUserExist *req = reinterpret_cast<Request_Auth_To_DB_IsUserExist*>(packet);

				wchar_t query[256];
				swprintf(query, sizeof(query), L"EXEC USER_IS_USER_EXIST '%wS'", req->username);

				ResultMapPtr result(m_db.Execute(query));

				Response_DB_To_Auth_IsUserExist res;
				
				res.user_uid = std::stoi(result->at(L"uuid"));
				res.RESPONSE_ID = req->RESPONSE_ID;

				Send(id, reinterpret_cast<char*>(&res));

				break;
			}
			case ID_Request_World_To_DB_GetUserStatus:
			{
				Request_World_To_DB_GetUserStatus *req = reinterpret_cast<Request_World_To_DB_GetUserStatus*>(packet);

				wchar_t query[256];
				swprintf(query, sizeof(query), L"EXEC GET_USER_INFO %d", req->user_uid);

				ResultMapPtr result(m_db.Execute(query));
				Response_DB_To_World_GetUserStatus res;
				res.RESPONSE_ID = req->RESPONSE_ID;
				res.user_uid = -1;

				if (result->size() > 0)
				{
					res.user_uid = std::stoi(result->at(L"uuid"));
					wcscpy_s(res.username, result->at(L"username").c_str());
					res.level = std::stoi(result->at(L"level"));
					res.exp = std::stol(result->at(L"exp"));
					res.hp = std::stoi(result->at(L"hp"));
					res.x = std::stoi(result->at(L"x"));
					res.y = std::stoi(result->at(L"y"));

					Send(id, reinterpret_cast<char*>(&res));
				}

				Send(id, reinterpret_cast<char*>(&res));

				break;
			}
		}
	}
}

void DBServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}

void DBServer::DBConnect(const wchar_t* ip, const wchar_t* scheme, const wchar_t* id, const wchar_t* pwd)
{
	m_db.Connect(ip, scheme, id, pwd);
}
