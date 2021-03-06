#include "Defines.h"
#include "BasePacket.h"

const unsigned char ID_LOGIN_OK = 10;
const unsigned char ID_LOGIN_FAIL = 11;
const unsigned char ID_POSITION_INFO = 12;
const unsigned char ID_NOTIFY_CHAT = 13;
const unsigned char ID_STAT_CHANGE = 14;
const unsigned char ID_REMOVE_OBJECT = 15;
const unsigned char ID_ADD_OBJECT = 16;
const unsigned char ID_CONNECT_SERVER = 17;
const unsigned char ID_Notify_Player_Enter = 18;
const unsigned char ID_Notify_Player_Move = 19;
const unsigned char ID_Notify_Player_Die = 20;
const unsigned char ID_Notify_Player_Info = 21;
const unsigned char ID_Notify_Player_HPRegen = 22;
const unsigned char ID_Notify_ChatMessage = 23;
const unsigned char ID_Notify_NPC_Move = 24;
const unsigned char ID_Notify_Player_Attack_NPC = 71;
const unsigned char ID_Notify_NPC_Attack_Player = 72;
const unsigned char ID_Notify_NPC_Damaged = 73;
#pragma pack(push, 1)
struct LOGIN_OK : BasePacket
{
    LOGIN_OK()
    {
        PACKET_ID = ID_LOGIN_OK;
        SIZE = sizeof(LOGIN_OK);
    }

    wchar_t username[12];
    unsigned int ID;
    unsigned short X_POS;
    unsigned short Y_POS;
    short HP;
    unsigned short LEVEL;
    unsigned long EXP;
    short max_hp;
    unsigned short base_damage;
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

    short HP;
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

    unsigned int ID;
};

struct ADD_OBJECT : BasePacket
{
    ADD_OBJECT()
    {
        PACKET_ID = ID_ADD_OBJECT;
        SIZE = sizeof(ADD_OBJECT);
    }

    unsigned int ID;
    char TYPE;
    unsigned short x;
    unsigned short y;
    wchar_t name[12];
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
    unsigned int user_uid;
};

struct Notify_Player_Attack_NPC : BasePacket
{
    Notify_Player_Attack_NPC()
    {
        PACKET_ID = ID_Notify_Player_Attack_NPC;
        SIZE = sizeof(Notify_Player_Attack_NPC);
    }
    unsigned int npc_id;
    unsigned short damage;
};

struct Notify_NPC_Attack_Player : BasePacket
{
    Notify_NPC_Attack_Player()
    {
        PACKET_ID = ID_Notify_NPC_Attack_Player;
        SIZE = sizeof(Notify_NPC_Attack_Player);
    }

    unsigned int npc_id;
    unsigned short damage;
};

struct Notify_Player_Enter : BasePacket
{
    Notify_Player_Enter()
    {
        PACKET_ID = ID_Notify_Player_Enter;
        SIZE = sizeof(Notify_Player_Enter);
    }

    unsigned int user_uid;
    wchar_t player_name[12];
    unsigned short x;
    unsigned short y;
};

struct Notify_Player_Move_Position : BasePacket
{
    Notify_Player_Move_Position()
    {
        PACKET_ID = ID_Notify_Player_Move;
        SIZE = sizeof(Notify_Player_Move_Position);
    }
    unsigned int id;
    unsigned short x;
    unsigned short y;
};

struct Notify_NPC_Move_Position : BasePacket
{
    Notify_NPC_Move_Position()
    {
        PACKET_ID = ID_Notify_NPC_Move;
        SIZE = sizeof(Notify_NPC_Move_Position);
    }
    unsigned int id;
    unsigned short x;
    unsigned short y;
};

struct Notify_NPC_Damaged : BasePacket
{
    Notify_NPC_Damaged()
    {
        PACKET_ID = ID_Notify_NPC_Damaged;
        SIZE = sizeof(Notify_NPC_Damaged);
    }
    
    unsigned int npc_id;
    short npc_hp;
    unsigned short gained_damage;
};

struct Notify_Player_Die : BasePacket
{
    Notify_Player_Die()
    {
        PACKET_ID = ID_Notify_Player_Die;
        SIZE = sizeof(Notify_Player_Die);
    }
    bool restart;
};

struct Notify_Player_Info : BasePacket
{
    Notify_Player_Info()
    {
        PACKET_ID = ID_Notify_Player_Info;
        SIZE = sizeof(Notify_Player_Info);
    }

    short HP;
    unsigned short LEVEL;
    unsigned long EXP;
    short max_hp;
    unsigned short base_damage;
};

struct Notify_Player_HPRegen : BasePacket
{
    Notify_Player_HPRegen()
    {
        PACKET_ID = ID_Notify_Player_HPRegen;
        SIZE = sizeof(Notify_Player_HPRegen);
    }
    
    short curr_hp;
};

struct Notify_ChatMessage : BasePacket
{
    Notify_ChatMessage()
    {
        PACKET_ID = ID_Notify_ChatMessage;
        SIZE = sizeof(Notify_ChatMessage);
    }
    
    unsigned int sender_id;
    wchar_t sender_name[12];
    wchar_t message[MAX_CHAT_MESSAGE_LENGTH];
};

#pragma pack(pop)
