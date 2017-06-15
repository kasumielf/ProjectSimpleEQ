#include "NonPlayer.h"


NonPlayer::~NonPlayer()
{
	lua_close(l);
}

void NonPlayer::InitLuaScript(const char * filename)
{
	l = luaL_newstate();
	luaL_openlibs(l);
	luaL_loadfile(l, filename);
	lua_pcall(l, 0, 0, 0);
}

void NonPlayer::DoLuaConversation(void* server_ptr, unsigned int player, char * msg)
{
	lua_getglobal(l, "DoConversation");
	lua_pushlightuserdata(l, server_ptr);
	lua_pushnumber(l, player);
	lua_pushnumber(l, 0);
	lua_pushstring(l, msg);
	lua_pcall(l, 4, 1, 0);
	lua_pop(l, 4);
}

void NonPlayer::SetRegenTime()
{
	next_regen_time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::milli>(respawn_time);
}

void NonPlayer::Update()
{
}
