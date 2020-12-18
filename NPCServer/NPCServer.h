#pragma once

#include <unordered_map>
#include <deque>

#include "../GSLibrary/BaseServer.h"
#include "NonPlayer.h"
#include "tinyxml2/tinyxml2.h"

extern "C"
{
#include <lua.h>
#include <lualib.h>
}
#include <lua.hpp>
#include <bitset>
#include <array>

#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"
#include "../Common/InnerNotifyPacket.h"

#include "tinyxml2/tinyxml2.h"

typedef std::bitset<8> Blocks;

class NPCServer : public BaseServer
{
private:
    Blocks block[MAX_WORLD_HEIGHT][MAX_WORLD_WIDTH];

    int last_add_npc_id;

    std::unordered_map<unsigned int, NonPlayer*> npcs;
    std::unordered_map<unsigned int, NonPlayer*> regen_npcs;
    std::unordered_map<unsigned int, Object*> players;

public:
    NPCServer(const int capacity, const short port);
    ~NPCServer();

    void CreateNPC(NonPlayer *npc);
    void ClearNPCs();
    void ClearPlayers();
    void NPCAttackUpdate(unsigned int id, NPCServer* self);
    void NPCRegen(unsigned int id, NPCServer* self);
    void NPCMove(unsigned int id, NPCServer* self);

    virtual void ProcessPacket(const int id, unsigned char* packet);
    virtual void Logging(const wchar_t* msg, ...);
    virtual void OnCloseSocket(const int id) {}

    void PlayerEntered(const int id, Notify_World_To_NPC_PlayerEntered * not);
    void PlayerExit(const int id, Notify_World_To_NPC_PlayerExit * not);
    void PlayerAttackNPC(const int id, Notify_World_To_NPC_PlayerAttackNPC * not);
    void PlayerMove(const int id, Notify_World_To_NPC_PlayerMove * not);
    void NPCStopAttackPlayer(const int id, Notify_World_To_NPC_NPCStopAttackPlayer * not);
    void PlayerSendMessage(const int id, Request_World_To_NPC_PlayerChat * req);
    
    void CreateNPCFromResource(const char* xmlfilename, unsigned short x, unsigned short y);
    void NPCMoveProcess(NonPlayer* npc, Object* player);

    bool IsClosed(short from_x, short from_y, short to_x, short to_y);

    std::array< std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT > GetBlockDataFromBitmap(const char* filename);
    void BlockCellInit(std::array< std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT > data);

};

//static int SYSTEM_Set_RespawnPosition(lua_State * l);
//static int SYSTEM_Send_Message(lua_State * l);
