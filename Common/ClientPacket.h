#pragma once

#include "BasePacket.h"

const unsigned char ID_LOGIN = 1;
const unsigned char ID_MOVE = 2;
const unsigned char ID_ATTACK = 3;
const unsigned char ID_CHAT = 4;
const unsigned char ID_LOGOUT = 5;

struct LOGIN : BasePacket
{
	LOGIN()
	{
		PACKET_ID = ID_LOGIN;
		SIZE = sizeof(LOGIN);
	}

	wchar_t ID_STR[12];
};

struct MOVE : BasePacket
{
	MOVE()
	{
		PACKET_ID = ID_MOVE;
		SIZE = sizeof(MOVE);
	}

	char DIR;
};

struct ATTACK : BasePacket
{
	ATTACK()
	{
		PACKET_ID = ID_ATTACK;
		SIZE = sizeof(ATTACK);
	}
};

struct CHAT : BasePacket
{
	CHAT()
	{
		PACKET_ID = ID_CHAT;
		SIZE = sizeof(CHAT);
	}

	wchar_t CHAT_STR[100];
};

struct LOGOUT : BasePacket
{
	LOGOUT()
	{
		PACKET_ID = ID_LOGOUT;
		SIZE = sizeof(LOGOUT);
	}
};
