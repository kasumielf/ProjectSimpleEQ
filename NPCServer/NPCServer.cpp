#include "NPCServer.h"
#include <iostream>

NPCServer::NPCServer(const int capacity, const short port) : BaseServer(capacity, port), last_add_npc_id(NPC_START_ID)
{
	AttachIOCPEvent(IOCPOpType::OpNPCAttack, std::bind(&NPCServer::NPCAttackUpdate, this, std::placeholders::_1, this));
	AttachIOCPEvent(IOCPOpType::OpNPCRegen, std::bind(&NPCServer::NPCRegen, this, std::placeholders::_1, this));
	AttachIOCPEvent(IOCPOpType::OpNPCMove, std::bind(&NPCServer::NPCMove, this, std::placeholders::_1, this));
}

NPCServer::~NPCServer()
{
}

void NPCServer::CreateNPCFromResource(const char * xmlfilename, unsigned short x, unsigned short y)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLNode *node;

	int error = doc.LoadFile(xmlfilename);

	if (error == 0)
	{
		int npc_id = last_add_npc_id + 1;
		NonPlayer *npc = new NonPlayer(npc_id, -1);

		wchar_t name[12];
		unsigned int level;
		unsigned int exp;
		unsigned int max_hp;
		unsigned int base_damage;
		unsigned int faction_group;
		double respawn_time;

		node = doc.FirstChild();

		tinyxml2::XMLElement *elem = node->FirstChildElement("Name");

		size_t cs;
		mbstowcs_s(&cs, name, 12, elem->Attribute("value"), 12);
		npc->SetName(name);

		npc->SetX(x);
		npc->SetY(y);

		elem = node->FirstChildElement("Level");
		elem->QueryUnsignedText(&level);
		elem = node->FirstChildElement("Exp");
		elem->QueryUnsignedText(&exp);
		elem = node->FirstChildElement("MaxHp");
		elem->QueryUnsignedText(&max_hp);
		elem = node->FirstChildElement("BaseDamage");
		elem->QueryUnsignedText(&base_damage);
		elem = node->FirstChildElement("FactionGroup");
		elem->QueryUnsignedText(&faction_group);
		elem = node->FirstChildElement("RespawnTime");
		elem->QueryDoubleText(&respawn_time);

		tinyxml2::XMLElement *scriptelem = node->FirstChildElement("DescriptScript");

		if (scriptelem != nullptr)
		{
			const char* filename = scriptelem->Attribute("src");
			npc->InitLuaScript(filename);
			lua_State *l = npc->GetLuaState();

			lua_register(l, "SYSTEM_Send_Message", SYSTEM_Send_Message);
			lua_register(l, "SYSTEM_Set_RespawnPosition", SYSTEM_Set_RespawnPosition);
		}

		npc->SetLevel(level);
		npc->SetExp(exp);
		npc->SetMaxHp(max_hp);
		npc->SetHP(max_hp);
		npc->SetBaseDamage(base_damage);
		npc->SetFactionGroup(faction_group);
		npc->SetRespawnTime(respawn_time);

		last_add_npc_id = npc_id;

		CreateNPC(npc);
	}
	else
	{
		Logging(L"tinyXML2 error : %d", error);
	}
}

void NPCServer::NPCMoveProcess(NonPlayer * npc, Object * player)
{
	Notify_NPC_To_World_NPCMove not;

	// Move Process


	not.npc_id = npc->GetId();
	not.x = npc->GetX();
	not.y = npc->GetY();

	SendToInternal("World", reinterpret_cast<unsigned char*>(&not));
}

void NPCServer::CreateNPC(NonPlayer * npc)
{
	Notify_NPC_To_World_NPCreatedAdd_NPC packet;

	packet.curr_hp = npc->GetHP();
	packet.faction_group = npc->GetFactionGroup();
	packet.id = npc->GetId();
	packet.level = npc->GetLevel();
	packet.max_hp = npc->GetMaxHp();
	packet.x = npc->GetX();
	packet.y = npc->GetY();

	wcscpy_s(packet.name, npc->GetName());

	npcs[npc->GetId()]= npc;

	SendToInternal("World", reinterpret_cast<unsigned char*>(&packet));

}

void NPCServer::ClearNPCs()
{
	auto iter_b = npcs.begin();
	auto iter_e = npcs.end();

	for (; iter_b != iter_e; ++iter_b)
	{
		delete &(iter_b);
	}

	npcs.clear();
	regen_npcs.clear();
}

void NPCServer::ClearPlayers()
{
	auto iter_b = players.begin();
	auto iter_e = players.end();

	for (; iter_b != iter_e; ++iter_b)
	{
		delete &(iter_b);
	}

	players.clear();
}


void NPCServer::NPCAttackUpdate(unsigned int id, NPCServer * self)
{
	if (self->npcs[id] != nullptr)
	{
		NonPlayer *npc = dynamic_cast<NonPlayer*>(self->npcs[id]);

		if (npc == nullptr || npc->GetCurrentState() != ObjectState::Battle)
			return;
	
		if (npc->GetCurrentState() == ObjectState::Battle)
		{
			unsigned short my_x = npc->GetX();
			unsigned short my_y = npc->GetY();

			unsigned short target_x = self->players[npc->GetAttackTarget()]->GetX();
			unsigned short target_y = self->players[npc->GetAttackTarget()]->GetX();

			if (self->IsClosed(my_x, my_y, target_x, target_y))
			{
				Notify_NPC_To_World_NPCAttackPlayer packet;
				packet.npc_id = npc->GetId();
				packet.target_id = npc->GetAttackTarget();
				packet.damage = npc->GetRealDamage();

				SendToInternal("World", reinterpret_cast<unsigned char*>(&packet));
			}
			else
			{
				Event move_event;
				move_event.provider = npc->GetId();
				move_event.event_type = IOCPOpType::OpNPCMove;
				self->PushTimerEvent(1000, move_event);
			}

			Event attack_event;
			attack_event.provider = id;
			attack_event.event_type = IOCPOpType::OpNPCAttack;

			self->PushTimerEvent(1000, attack_event);
		}
	}
}

void NPCServer::NPCRegen(unsigned int id, NPCServer * self)
{
	NonPlayer* npc = self->regen_npcs[id];

	if (npc == nullptr)
		return;

	self->Lock();
	self->CreateNPC(npc);
	self->npcs[id] = npc;
	self->regen_npcs.erase(id);
	self->Unlock();
}

void NPCServer::NPCMove(unsigned int id, NPCServer * self)
{
	NonPlayer* npc = self->npcs[id];
	if (npc == nullptr)
		return;

	self->NPCMoveProcess(npc, self->players[npc->GetAttackTarget]);
}

void NPCServer::ProcessPacket(const int id, unsigned char * packet)
{
	unsigned char packet_id = packet[0];

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_Notify_World_To_NPC_PlayerEntered:
			{
				PlayerEntered(id, reinterpret_cast<Notify_World_To_NPC_PlayerEntered*>(packet));
				break;
			}
			case ID_Notify_World_To_NPC_PlayerExit:
			{
				PlayerExit(id, reinterpret_cast<Notify_World_To_NPC_PlayerExit*>(packet));
				break;
			}
			case ID_Notify_World_To_NPC_PlayerAttackNPC:
			{
				PlayerAttackNPC(id, reinterpret_cast<Notify_World_To_NPC_PlayerAttackNPC*>(packet));
				break;
			}
			case ID_Notify_World_To_NPC_PlayerMove:
			{
				PlayerMove(id, reinterpret_cast<Notify_World_To_NPC_PlayerMove*>(packet));
				break;
			}
			case ID_Notify_World_To_NPC_NPCStopAttackPlayer:
			{
				NPCStopAttackPlayer(id, reinterpret_cast<Notify_World_To_NPC_NPCStopAttackPlayer*>(packet));
				break;
			}
			case ID_Request_World_To_NPC_PlayerChat:
			{
				PlayerSendMessage(id, reinterpret_cast<Request_World_To_NPC_PlayerChat*>(packet));
				break;
			}
		}
	}
}

void NPCServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}

void NPCServer::PlayerEntered(const int id, Notify_World_To_NPC_PlayerEntered * not)
{
	Object* player = new Object(not->player_id, ObjectType::Player);

	player->SetX(not->x);
	player->SetY(not->y);

	players.insert(std::make_pair(player->GetId(), player));
}

void NPCServer::PlayerExit(const int id, Notify_World_To_NPC_PlayerExit * not)
{
	if (players.count(not->player_id))
	{
		players.erase(not->player_id);
	}
}

void NPCServer::PlayerAttackNPC(const int id, Notify_World_To_NPC_PlayerAttackNPC * not)
{
	if (npcs.count(not->npc_id) > 0)
	{
		Lock();
		NonPlayer *npc = npcs[not->npc_id];

		if (npc->GetCurrentState() == ObjectState::Idle)
		{
			npc->SetCurrentState(ObjectState::Battle);
			npc->SetAttackTarget(not->attacker_id);

			Event attack_event;
			attack_event.provider = not->npc_id;
			attack_event.event_type = IOCPOpType::OpNPCAttack;

			PushTimerEvent(1000, attack_event);

		}

		int hp = npc->GetHP();

		if (hp - not->damage <= 0)
		{
			Notify_NPC_To_World_NPCDieFromPlayer die_notify;
			die_notify.npc_id = npc->GetId();
			die_notify.gained_exp = npc->GetExp();
			die_notify.remover_id = not->attacker_id;

			//delete npcs.at(not->npc_id);
			regen_npcs[npc->GetId()] = npc;
			npcs.erase(npc->GetId());

			Event regen_event;
			regen_event.provider = npc->GetId();
			regen_event.event_type = IOCPOpType::OpNPCRegen;

			PushTimerEvent(npc->GetRespawnTime(), regen_event);

			Send(id, reinterpret_cast<unsigned char*>(&die_notify));
		}
		else
		{
			hp -= not->damage;

			Notify_NPC_To_World_NPCDamaged damaged_notify;
			damaged_notify.gained_damage = not->damage;
			damaged_notify.npc_hp = npc->GetHP();
			damaged_notify.npc_id = npc->GetId();

			Send(id, reinterpret_cast<unsigned char*>(&damaged_notify));
			npc->SetHP(hp);
		}

		Unlock();
	}
}

void NPCServer::PlayerMove(const int id, Notify_World_To_NPC_PlayerMove * not)
{
	if (players.count(not->player_id) > 0)
	{
		players[not->player_id]->SetX(not->x);
		players[not->player_id]->SetY(not->y);
	}
}

void NPCServer::NPCStopAttackPlayer(const int id, Notify_World_To_NPC_NPCStopAttackPlayer * not)
{
	npcs[not->npc_id]->SetCurrentState(ObjectState::Idle);
	npcs[not->npc_id]->SetAttackTarget(0);
}

void NPCServer::PlayerSendMessage(const int id, Request_World_To_NPC_PlayerChat * req)
{
	Object *p = players[req->sender_id];
	NonPlayer *np = npcs[req->target_id];

	if (IsClosed(np->GetX(), np->GetY(), p->GetX(), p->GetY()))
	{
		//ToDO Chat Script
		Response_NPC_To_World_NPCMessage res;
		res.RESPONSE_ID = req->sender_id;
		res.npc_id = np->GetId();

		char* msg;

		wcstombs(msg, req->message, MAX_CHAT_MESSAGE_LENGTH);

		if (np->GetLuaState() != nullptr)
			np->DoLuaConversation((void*)this, p->GetId(), msg);

		Send(id, reinterpret_cast<unsigned char*>(&res));
	}
}

bool NPCServer::IsClosed(short from_x, short from_y, short to_x, short to_y)
{
	return (from_x - to_x) * (from_x - to_x) + (from_y - to_y) * (from_y - to_y) <= 1 * 1;
}

int SYSTEM_Set_RespawnPosition(lua_State * l)
{
	NPCServer *cls = (NPCServer*)(lua_touserdata(l, -2));
	int player_id = (int)lua_tonumber(l, -1);

	std::cout << "bind" << std::endl;

	return 0;
}

int SYSTEM_Send_Message(lua_State * l)
{
	NPCServer *cls = (NPCServer*)(lua_touserdata(l, -3));
	int player_id = (int)lua_tonumber(l, -2);
	const char* answer = lua_tostring(l, -3);
	lua_pop(l, 8);

	std::cout << "anser : " << std::endl;

	return 0;
}
