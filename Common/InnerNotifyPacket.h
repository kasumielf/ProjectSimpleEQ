#pragma once

#include "InnerBasePacket.h"

// for NPC
const unsigned char ID_Notify_NPC_To_World_NPCreated = 100;
const unsigned char ID_Notify_NPC_To_World_NPCAttackPlayer = 101;
const unsigned char ID_Notify_NPC_To_World_NPCDamaged = 102;
const unsigned char ID_Notify_NPC_To_World_NPCDieFromPlayer = 103;
const unsigned char ID_Notify_NPC_To_World_NPCMove = 104;
const unsigned char ID_Notify_NPC_To_World_NPCRemoved = 105;

const unsigned char ID_Notify_World_To_NPC_PlayerEntered = 106;
const unsigned char ID_Notify_World_To_NPC_PlayerExit = 107;
const unsigned char ID_Notify_World_To_NPC_PlayerAttackNPC = 108;
const unsigned char ID_Notify_World_To_NPC_PlayerMove = 109;
const unsigned char ID_Notify_World_To_NPC_NPCStopAttackPlayer = 110;


#pragma pack(push, 1)

struct Notify_NPC_To_World_NPCreatedAdd_NPC : InnerBasePacket
{
	Notify_NPC_To_World_NPCreatedAdd_NPC()
	{
		PACKET_ID = ID_Notify_NPC_To_World_NPCreated;
		SIZE = sizeof(Notify_NPC_To_World_NPCreatedAdd_NPC);
	}

	unsigned int id;
	wchar_t name[12];
	unsigned int level;
	unsigned int curr_hp;
	unsigned int max_hp;
	unsigned int faction_group;
	unsigned short x;
	unsigned short y;
};

struct Notify_NPC_To_World_NPCAttackPlayer : InnerBasePacket
{
	Notify_NPC_To_World_NPCAttackPlayer()
	{
		PACKET_ID = ID_Notify_NPC_To_World_NPCAttackPlayer;
		SIZE = sizeof(Notify_NPC_To_World_NPCAttackPlayer);
	}
	unsigned int npc_id;
	unsigned int target_id;
	unsigned int damage;
};

struct Notify_NPC_To_World_NPCDamaged : InnerBasePacket
{
	Notify_NPC_To_World_NPCDamaged()
	{
		PACKET_ID = ID_Notify_NPC_To_World_NPCDamaged;
		SIZE = sizeof(Notify_NPC_To_World_NPCDamaged);
	}
	unsigned int npc_id;
	unsigned int gained_damage;
	unsigned int npc_hp;
};

struct Notify_NPC_To_World_NPCDieFromPlayer : InnerBasePacket
{
	Notify_NPC_To_World_NPCDieFromPlayer()
	{
		PACKET_ID = ID_Notify_NPC_To_World_NPCDieFromPlayer;
		SIZE = sizeof(Notify_NPC_To_World_NPCDieFromPlayer);
	}
	unsigned int npc_id; unsigned int remover_id;
	unsigned long gained_exp;
};

struct Notify_NPC_To_World_NPCMove : InnerBasePacket
{
	Notify_NPC_To_World_NPCMove()
	{
		PACKET_ID = ID_Notify_NPC_To_World_NPCMove;
		SIZE = sizeof(Notify_NPC_To_World_NPCMove);
	}

	unsigned int npc_id;
	unsigned short x;
	unsigned short y;
};

struct Notify_World_To_NPC_PlayerEntered : InnerBasePacket
{
	Notify_World_To_NPC_PlayerEntered()
	{
		PACKET_ID = ID_Notify_World_To_NPC_PlayerEntered;
		SIZE = sizeof(Notify_World_To_NPC_PlayerEntered);
	}
	unsigned int player_id;
	unsigned short x;
	unsigned short y;
};

struct Notify_World_To_NPC_PlayerExit : InnerBasePacket
{
	Notify_World_To_NPC_PlayerExit()
	{
		PACKET_ID = ID_Notify_World_To_NPC_PlayerEntered;
		SIZE = sizeof(Notify_World_To_NPC_PlayerExit);
	}
	unsigned int player_id;
};

struct Notify_World_To_NPC_PlayerMove : InnerBasePacket
{
	Notify_World_To_NPC_PlayerMove()
	{
		PACKET_ID = ID_Notify_World_To_NPC_PlayerMove;
		SIZE = sizeof(Notify_World_To_NPC_PlayerMove);
	}
	unsigned int player_id;
	unsigned int x;
	unsigned int y;
};


struct Notify_World_To_NPC_PlayerAttackNPC : InnerBasePacket
{
	Notify_World_To_NPC_PlayerAttackNPC()
	{
		PACKET_ID = ID_Notify_World_To_NPC_PlayerAttackNPC;
		SIZE = sizeof(Notify_World_To_NPC_PlayerAttackNPC);
	}

	unsigned int attacker_id;
	unsigned int npc_id;
	unsigned short damage;
};


struct Notify_World_To_NPC_NPCStopAttackPlayer : InnerBasePacket
{
	Notify_World_To_NPC_NPCStopAttackPlayer()
	{
		PACKET_ID = ID_Notify_World_To_NPC_NPCStopAttackPlayer;
		SIZE = sizeof(Notify_World_To_NPC_NPCStopAttackPlayer);
	}

	unsigned int npc_id;
	unsigned int target_id;
};


#pragma pack(pop)
