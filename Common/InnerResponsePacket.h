#pragma once

#include "InnerBasePacket.h"

// for DB
const unsigned char ID_Response_DB_To_Auth_UserExist = 50;
const unsigned char ID_Response_DB_To_World_GetUserStatus = 51;
const unsigned char ID_Response_DB_To_World_UpdateUserStatus = 52;
// for world
const unsigned char ID_Response_World_To_Auth_AllocateUser = 53;
const unsigned char ID_Response_NPC_To_World_NPCMessage = 54;

#pragma pack(push, 1)
struct Response_DB_To_Auth_IsUserExist : InnerBasePacket
{
	Response_DB_To_Auth_IsUserExist()
	{
		PACKET_ID = ID_Response_DB_To_Auth_UserExist;
		SIZE = sizeof(Response_DB_To_Auth_IsUserExist);
	}

	unsigned int user_uid;
	unsigned int client_id;
};

struct Response_DB_To_World_GetUserStatus : InnerBasePacket
{
	Response_DB_To_World_GetUserStatus()
	{
		PACKET_ID = ID_Response_DB_To_World_GetUserStatus;
		SIZE = sizeof(Response_DB_To_World_GetUserStatus);
	}
	unsigned int client_id;
	unsigned int user_uid;
	wchar_t username[12];
	unsigned short x;
	unsigned short y;
	unsigned short hp;
	unsigned short max_hp;
	unsigned short level;
	unsigned long exp;
	unsigned short start_x;
	unsigned short start_y;

};

struct Response_DB_To_World_UpdateUserStatus : InnerBasePacket
{
	Response_DB_To_World_UpdateUserStatus()
	{
		PACKET_ID = ID_Response_DB_To_World_UpdateUserStatus;
		SIZE = sizeof(Response_DB_To_World_UpdateUserStatus);
	}

	bool result;
};

struct Response_World_To_Auth_AllocateUser : InnerBasePacket
{
	Response_World_To_Auth_AllocateUser()
	{
		PACKET_ID = ID_Response_World_To_Auth_AllocateUser;
		SIZE = sizeof(Response_World_To_Auth_AllocateUser);
	}

	bool success;
	unsigned int client_id;
	unsigned int user_uid;
};

struct Response_NPC_To_World_NPCMessage : InnerBasePacket
{
	Response_NPC_To_World_NPCMessage()
	{
		PACKET_ID = ID_Response_NPC_To_World_NPCMessage;
		SIZE = sizeof(Response_NPC_To_World_NPCMessage);
	}
	unsigned int npc_id;
	wchar_t message[MAX_CHAT_MESSAGE_LENGTH];
};

#pragma pack(pop)
