#pragma once

#include "InnerBasePacket.h"

// for DB
const unsigned char ID_Response_User_Exist = 150;
const unsigned char ID_Response_DB_To_World_GetUserStatus = 151;
const unsigned char ID_Response_DB_To_World_UpdateUserStatus = 151;

// for NPC
const unsigned char ID_Response_Attack_NPC = 152;
const unsigned char ID_Notify_Add_NPC = 153;
const unsigned char ID_Notify_Remove_NPC = 154;
const unsigned char ID_Notify_NPC_Attack = 155;

// for world
const unsigned char ID_Response_World_To_Auth_AllocateUser = 180;


struct Response_DB_To_Auth_IsUserExist : InnerBasePacket
{
	Response_DB_To_Auth_IsUserExist()
	{
		PACKET_ID = ID_Response_User_Exist;
		SIZE = sizeof(Response_DB_To_Auth_IsUserExist);
	}

	unsigned int user_uid;
};

struct Response_DB_To_World_GetUserStatus : InnerBasePacket
{
	Response_DB_To_World_GetUserStatus()
	{
		PACKET_ID = ID_Response_DB_To_World_GetUserStatus;
		SIZE = sizeof(Response_DB_To_World_GetUserStatus);
	}

	unsigned short user_uid;
	wchar_t username[12];
	unsigned short x;
	unsigned short y;
	unsigned short hp;
	unsigned short level;
	unsigned long exp;
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

struct Response_Attack_NPC : InnerBasePacket
{
	Response_Attack_NPC()
	{
		PACKET_ID = ID_Response_Attack_NPC;
		SIZE = sizeof(Response_Attack_NPC);
	}

	unsigned short attacker_id;
	unsigned short npc_id;
	unsigned short damage;
};

struct Notify_NPC_To_World_AddNPC : InnerBasePacket
{
	Notify_NPC_To_World_AddNPC()
	{
		PACKET_ID = ID_Notify_Add_NPC;
		SIZE = sizeof(Notify_NPC_To_World_AddNPC);
	}

	unsigned short npc_id;
	unsigned char npc_type;
};

struct Notify_NPC_To_World_RemoveNPC : InnerBasePacket
{
	Notify_NPC_To_World_RemoveNPC()
	{
		PACKET_ID = ID_Notify_Remove_NPC;
		SIZE = sizeof(Notify_NPC_To_World_RemoveNPC);
	}

	unsigned short npc_id;
};

struct Notify_NPC_To_World_NPCAttack : InnerBasePacket
{
	Notify_NPC_To_World_NPCAttack()
	{
		PACKET_ID = ID_Notify_NPC_Attack;
		SIZE = sizeof(Notify_NPC_To_World_NPCAttack);
	}

	unsigned short npc_id;
	unsigned short target_id;
	unsigned short damage;
};

struct Response_World_To_Auth_AllocateUser : InnerBasePacket
{
	Response_World_To_Auth_AllocateUser()
	{
		PACKET_ID = ID_Response_World_To_Auth_AllocateUser;
		SIZE = sizeof(Response_World_To_Auth_AllocateUser);
	}

	bool success;
};