#include "Defines.h"
#include "BasePacket.h"

const short ID_LOGIN_OK = 1;
const short ID_LOGIN_FAIL = 2;
const short ID_POSITION_INFO = 3;
const short ID_NOTIFY_CHAT = 4;
const short ID_STAT_CHANGE = 5;
const short ID_REMOVE_OBJECT = 6;
const short ID_ADD_OBJECT = 7;
const short ID_CONNECT_SERVER = 255;

struct LOGIN_OK : BasePacket
{
	LOGIN_OK()
	{
		PACKET_ID = ID_LOGIN_OK;
		SIZE = sizeof(LOGIN_OK);
	}

	wchar_t username[12];
	unsigned short ID;
	unsigned short X_POS;
	unsigned short Y_POS;
	unsigned short HP;
	unsigned short LEVEL;
	unsigned long EXP;
};

struct LOGIN_FAIL : BasePacket
{
	LOGIN_FAIL()
	{
		PACKET_ID = ID_LOGIN_FAIL;
		SIZE = sizeof(LOGIN_FAIL);
	}

};

struct POSITION_INFO : BasePacket
{
	POSITION_INFO()
	{
		PACKET_ID = ID_POSITION_INFO;
		SIZE = sizeof(POSITION_INFO);
	}

	unsigned short ID;
	unsigned short X_POS;
	unsigned short Y_POS;
};


struct Notify_Chat : BasePacket
{
	Notify_Chat()
	{
		PACKET_ID = ID_NOTIFY_CHAT;
		SIZE = sizeof(Notify_Chat);
	}
	unsigned short speaker_id;
	wchar_t CHAT_STR[100];
};

struct STAT_CHANGE : BasePacket
{
	STAT_CHANGE()
	{
		PACKET_ID = ID_STAT_CHANGE;
		SIZE = sizeof(STAT_CHANGE);
	}

	unsigned short HP;
	unsigned short LEVEL;
	unsigned long EXP;
};

struct REMOVE_OBJECT : BasePacket
{
	REMOVE_OBJECT()
	{
		PACKET_ID = ID_REMOVE_OBJECT;
		SIZE = sizeof(REMOVE_OBJECT);
	}

	unsigned short ID;
};

struct ADD_OBJECT : BasePacket
{
	ADD_OBJECT()
	{
		PACKET_ID = ID_ADD_OBJECT;
		SIZE = sizeof(ADD_OBJECT);
	}

	unsigned short ID;
	char TYPE;
	unsigned short x;
	unsigned short y;
};

struct CONNECT_SERVER : BasePacket
{
	CONNECT_SERVER()
	{
		PACKET_ID = ID_CONNECT_SERVER;
		SIZE = sizeof(CONNECT_SERVER);
	}

	wchar_t ip[15];
	unsigned short port;
};

