#include <iostream>
#include <locale>

#include  "NPCServer.h"
extern "C"
{
#include <lua.h>
#include <lualib.h>
}
#include <lua.hpp>

NPCServer server(5000, 4002);

int CreateNPCFromLuaScript(lua_State* l)
{
	char* filename = (char*)lua_tostring(l, -3);
	int x = lua_tonumber(l, -2);
	int y = lua_tonumber(l, -1);
	lua_pop(l, 3);

	server.CreateNPCFromResource(filename, x, y);
	server.Logging(L"[NPC Create] filename : %S, Position X : %d, Y : %d", filename, x, y);
	return 0;
}

int main(int argc, char** argv)
{
	_wsetlocale(LC_ALL, L"kor");

	server.Init();
	server.Start();
	Sleep(5000);
	server.Connect("World", "127.0.0.1", 4003);
	Sleep(5000);

	lua_State *l = luaL_newstate();
	luaL_openlibs(l);
	luaL_loadfile(l, "Monsters\\InitMonster.lua");
	if (lua_pcall(l, 0, 0, 0) != 0)
	{
		std::cout << "Lua Error : " << (char *)lua_tostring(l, -1) << std::endl;
	}
	lua_register(l, "SYSTEM_CreateNPC", CreateNPCFromLuaScript);
	lua_getglobal(l, "Init");
	lua_pcall(l, 0, 1, 0);
	lua_pop(l, 0);
	lua_close(l);

	while (true)
	{
	}
}

