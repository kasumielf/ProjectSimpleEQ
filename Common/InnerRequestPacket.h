#pragma once

#include "InnerBasePacket.h"

// for DB Server
const unsigned char ID_Request_World_To_DB_UpdateUserPosition = 21;
const unsigned char ID_Request_World_To_DB_UpdateUserStatus = 22;
const unsigned char ID_Request_Auth_To_DB_IsUserExist = 23;
const unsigned char ID_Request_World_To_DB_GetUserStatus = 24;

// for NPC Server
const unsigned char ID_Request_NPC_To_World_NPCMove = 25;

// etc
const unsigned char ID_Request_Auth_To_World_AllocateUser = 26;

#pragma pack(push, 1)
struct Request_Auth_To_DB_IsUserExist : InnerBasePacket
{
	Request_Auth_To_DB_IsUserExist()
	{
		PACKET_ID = ID_Request_Auth_To_DB_IsUserExist;
		SIZE = sizeof(Request_Auth_To_DB_IsUserExist);
	}

	wchar_t username[12];
};

struct Request_World_To_DB_UpdateUserPotision : InnerBasePacket
{
	Request_World_To_DB_UpdateUserPotision()
	{
		PACKET_ID = ID_Request_World_To_DB_UpdateUserPosition;
		SIZE = sizeof(Request_World_To_DB_UpdateUserPotision);
	}

	unsigned int user_uid;
	unsigned short x;
	unsigned short y;
};

struct Request_World_To_DB_UpdateUserStatus : InnerBasePacket
{
	Request_World_To_DB_UpdateUserStatus()
	{
		PACKET_ID = ID_Request_World_To_DB_UpdateUserStatus;
		SIZE = sizeof(Request_World_To_DB_UpdateUserStatus);
	}

	unsigned int user_uid;
	unsigned short x;
	unsigned short y;
	unsigned short hp;
	unsigned short level;
	unsigned long exp;
};

struct Request_World_To_DB_GetUserStatus : InnerBasePacket
{
	Request_World_To_DB_GetUserStatus()
	{
		PACKET_ID = ID_Request_World_To_DB_GetUserStatus;
		SIZE = sizeof(Request_World_To_DB_GetUserStatus);
	}

	unsigned int client_id;
	unsigned int user_uid;
};

struct Request_Auth_To_World_AllocateUser : InnerBasePacket
{
	Request_Auth_To_World_AllocateUser()
	{
		PACKET_ID = ID_Request_Auth_To_World_AllocateUser;
		SIZE = sizeof(Request_Auth_To_World_AllocateUser);
	}

	unsigned int user_uid;
	unsigned int client_id;
};

struct Request_NPC_To_World_NPCMoveAdd_NPC : InnerBasePacket
{
	Request_NPC_To_World_NPCMoveAdd_NPC()
	{
		PACKET_ID = ID_Request_NPC_To_World_NPCMove;
		SIZE = sizeof(Request_NPC_To_World_NPCMoveAdd_NPC);
	}
};

#pragma pack(pop)
