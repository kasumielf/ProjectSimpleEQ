#include "NonPlayer.h"
#include <iostream>

NonPlayer::~NonPlayer()
{
    lua_close(l);
}

void NonPlayer::InitLuaScript(const char * filename)
{
    l = luaL_newstate();
    luaL_openlibs(l);
    luaL_loadfile(l, filename);
    if (lua_pcall(l, 0, 0, 0) != 0)
    {
        std::cout << "Lua Error : " << (char *)lua_tostring(l, -1) << std::endl;
    }
    else
    {
        has_script = true;
    }
}

void NonPlayer::DoLuaConversation(void* server_ptr, unsigned int player, char * msg)
{
    lua_getglobal(l, "DoConversation");
    lua_pushlightuserdata(l, server_ptr);
    lua_pushnumber(l, GetId());
    lua_pushnumber(l, player);
    lua_pushnumber(l, 0);
    lua_pushstring(l, msg);
    if (lua_pcall(l, 5, 1, 0) != 0)
    {
        std::cout << "Lua Error : " << (char *)lua_tostring(l, -1) << std::endl;
    }
    //lua_pop(l, 5);
}

void NonPlayer::MakePath(std::bitset<8>& blocks)
{
    startNode->x = GetX();
    startNode->y = GetY();

    endNode->x = target_x;
    endNode->y = target_y;

    move_paths = path_seracher.FindPath(startNode, endNode, blocks);
}

void NonPlayer::SetRegenTime()
{
    next_regen_time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::milli>(respawn_time);
}

void NonPlayer::Update()
{
}
