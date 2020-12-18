#include "DBServer.h"
#include <iostream>

#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"

void DBServer::ProcessPacket(const int id, unsigned char * packet)
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

                ResultMap* result = m_db.Execute(query);

                Response_DB_To_Auth_IsUserExist res;
                res.client_id = req->RESPONSE_ID;
                res.user_uid = 0;

                if (result->size() > 0)
                {
                    res.user_uid = std::stoi(result->at(L"uuid"));
                    res.RESPONSE_ID = req->RESPONSE_ID;
                }

                Send(id, reinterpret_cast<unsigned char*>(&res));

                delete result;

                break;
            }
            case ID_Request_World_To_DB_GetUserStatus:
            {
                Request_World_To_DB_GetUserStatus *req = reinterpret_cast<Request_World_To_DB_GetUserStatus*>(packet);

                wchar_t query[256];
                swprintf(query, sizeof(query), L"EXEC USER_GET_USERINFO %d", req->user_uid);

                ResultMapPtr result(m_db.Execute(query));
                Response_DB_To_World_GetUserStatus res;
                res.RESPONSE_ID = req->RESPONSE_ID;
                res.user_uid = 0;
                res.client_id = req->client_id;

                if (result->size() > 0)
                {
                    res.user_uid = std::stoul(result->at(L"uuid"));
                    wcscpy_s(res.username, result->at(L"username").c_str());
                    res.level = std::stoi(result->at(L"level"));
                    res.exp = std::stol(result->at(L"exp"));
                    res.hp = std::stoi(result->at(L"hp"));
                    res.max_hp = std::stoi(result->at(L"maxhp"));
                    res.x = std::stoi(result->at(L"x"));
                    res.y = std::stoi(result->at(L"y"));
                    res.start_x = std::stoi(result->at(L"start_x"));
                    res.start_y = std::stoi(result->at(L"start_y"));
                }

                Send(id, reinterpret_cast<unsigned char*>(&res));

                break;
            }
            case ID_Request_World_To_DB_UpdateUserStatus:
            {
                Request_World_To_DB_UpdateUserStatus *req = reinterpret_cast<Request_World_To_DB_UpdateUserStatus*>(packet);
                wchar_t query[256];

                swprintf(query, sizeof(query), L"EXEC USER_UPDATE_USERINFO %d, %d, %d, %d, %d, %d, %d",
                    req->user_uid, req->level, req->exp, req->hp, req->max_hp, req->x, req->y);
                m_db.ExecuteDirect(query);

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
