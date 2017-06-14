#include "NPCServer.h"
#include <iostream>
#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"
#include "../Common/InnerNotifyPacket.h"

NPCServer::NPCServer(const int capacity, const short port) : BaseServer(capacity, port), last_add_npc_id(NPC_START_ID)
{
	AttachIOCPEvent(IOCPOpType::OpNPCAttack, std::bind(&NPCServer::NPCAttackUpdate, this, std::placeholders::_1, this));
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

		if (x > 0 && y > 0)
		{
			npc->SetX(x);
			npc->SetY(y);
		}
		else
		{
			tinyxml2::XMLElement *elem = node->FirstChildElement("Position");
			npc->SetX(atoi(elem->Attribute("x")));
			npc->SetY(atoi(elem->Attribute("y")));
		}

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
	regenQueue.clear();
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

void NPCServer::InitTemporaryNPCs()
{
	//lua_State *l = luaL_newstate();
	//luabind::open(l);
//	luabind::module(l)
//	[
//		luabind::def("CreateNPC", &NPCServer::CreateNPC)
//	];

	CreateNPCFromResource("Monsters/Captain001.xml", 0, 0);
}

void NPCServer::NPCAttackUpdate(unsigned int id, NPCServer * self)
{
	if (self->npcs[id] != nullptr)
	{
		NonPlayer *p = dynamic_cast<NonPlayer*>(self->npcs[id]);

		if (p == nullptr || p->GetCurrentState() != ObjectState::Battle)
			return;
	
		if (p->GetCurrentState() == ObjectState::Battle)
		{
			unsigned short my_x = p->GetX();
			unsigned short my_y = p->GetY();

			unsigned short target_x = self->players[p->GetAttackTarget()]->GetX();
			unsigned short target_y = self->players[p->GetAttackTarget()]->GetX();

			if (self->IsClosed(my_x, my_y, target_x, target_y))
			{
				Notify_NPC_To_World_NPCAttackPlayer packet;
				packet.npc_id = p->GetId();
				packet.target_id = p->GetAttackTarget();
				packet.damage = p->GetRealDamage();

				SendToInternal("World", reinterpret_cast<unsigned char*>(&packet));
			}
			else
			{
				// ToDo : 플레이어 따라가기	
			}

			Event attack_event;
			attack_event.provider = id;
			attack_event.event_type = IOCPOpType::OpNPCAttack;

			self->PushTimerEvent(1000, attack_event);
		}
	}
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

		unsigned int hp = npc->GetHP();

		if (hp - not->damage <= 0)
		{
			Notify_NPC_To_World_NPCDieFromPlayer die_notify;
			die_notify.npc_id = npc->GetId();
			die_notify.gained_exp = npc->GetExp();
			die_notify.remover_id = not->attacker_id;

			delete npcs.at(not->npc_id);
			npcs.erase(not->npc_id);

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
		wcscpy_s(res.message, L"안녕하세요! 좋은 아침입니다!");

		Send(id, reinterpret_cast<unsigned char*>(&res));
	}
}

bool NPCServer::IsClosed(short from_x, short from_y, short to_x, short to_y)
{
	return (from_x - to_x) * (from_x - to_x) + (from_y - to_y) * (from_y - to_y) <= 1 * 1;
}
