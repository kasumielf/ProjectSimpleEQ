#pragma once

#include "../GSLibrary/BaseServer.h"
#include "World.h"
#include "Status.h"

#include <vector>


class WorldServer :
	public BaseServer
{
private:
	World *world;
	std::vector<Player*> players;
	std::unordered_map<unsigned int, unsigned int> socketIds;
	std::vector<Status> level_data;
public:
	WorldServer(const int capacity, const short port);
	~WorldServer();

	virtual void ProcessPacket(const int id, unsigned char* packet);
	virtual void Logging(const wchar_t* msg, ...);
	virtual void OnCloseSocket(const int id);

	World* const GetWorldPtr() { return world; }
	
	void PlayerAttackUpdate(unsigned int id, WorldServer* self);
	void PlayerUpdate(unsigned int id, WorldServer* self);
	void PlayerDBSave(unsigned int id, WorldServer * self);

	bool IsClosed(short from_x, short from_y, short to_x, short to_y);
	Status& GetLevelData(unsigned int level) { return level_data[level]; }

	void InitStatusTable();
	void MovePlayer(int id, Player* p);

	void AllocateUser(const int id, Request_Auth_To_World_AllocateUser* req);
	void GetUserState(const int id, Response_DB_To_World_GetUserStatus* res);
	void EnterGameWorld(const int id, Request_Enter_GameWorld* req);
	void Move(const int id, MOVE* req);
	void Attack(const int id, ATTACK* req);
	void Logout(const int id, LOGOUT* req);
	void NPCCreated(const int id, Notify_NPC_To_World_NPCreatedAdd_NPC * not);
	void NPCDieFromPlayer(const int id, Notify_NPC_To_World_NPCDieFromPlayer * not);
	void NPCDamaged(const int id, Notify_NPC_To_World_NPCDamaged * not);
	void NPCAttackPlayer(const int id, Notify_NPC_To_World_NPCAttackPlayer * not);
	void SendChatMessage(const int id, Request_Send_MyChat* req);
	void NotifyNPCMesage(const int id, Response_NPC_To_World_NPCMessage* res);
};

