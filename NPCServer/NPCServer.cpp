#include "NPCServer.h"
#include <iostream>
#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"
#include "../Common/InnerNotifyPacket.h"

NPCServer::~NPCServer()
{
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
	// 임시 테스트용 NPC를 생성하는 코드

	const int temporary_npc_count = 1000;

	for(int i=0;i<temporary_npc_count;++i)
	{
		int id = i + 20000;
		NonPlayer* np = new NonPlayer(id, 60000);

		np->SetCurrentState(ObjectState::Idle);
		np->SetName(L"테스트 NPC");
		np->SetLevel(rand() % 5);
		np->SetBaseDamage(100);
		np->SetMaxHp(np->GetLevel() * 200);
		np->SetHP(np->GetMaxHp());
		np->SetFactionGroup(100);
		np->SetX(rand() % MAX_WORLD_WIDTH);
		np->SetY(rand() % MAX_WORLD_HEIGHT);

		npcs[id] = np;

		Notify_NPC_To_World_NPCreatedAdd_NPC packet;

		packet.curr_hp = np->GetHP();
		packet.faction_group = np->GetFactionGroup();
		packet.id = np->GetId();
		packet.level = np->GetLevel();
		packet.max_hp = np->GetMaxHp();
		packet.x = np->GetX();
		packet.y = np->GetY();

		wcscpy_s(packet.name, np->GetName());
		SendToInternal("World", reinterpret_cast<unsigned char*>(&packet));
		Sleep(10);
	};
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
				Notify_World_To_NPC_PlayerEntered *not = reinterpret_cast<Notify_World_To_NPC_PlayerEntered*>(packet);
				
				Object* player = new Object(not->player_id, ObjectType::Player);

				player->SetX(not->x);
				player->SetY(not->y);

				players.insert(std::make_pair(player->GetId(), player));
				break;
			}
			case ID_Notify_World_To_NPC_PlayerExit:
			{
				Notify_World_To_NPC_PlayerExit * not = reinterpret_cast<Notify_World_To_NPC_PlayerExit*>(packet);

				if (players.count(not->player_id))
				{
					players.erase(not->player_id);
				}

				break;
			}
			case ID_Notify_World_To_NPC_PlayerAttackNPC:
			{
				Notify_World_To_NPC_PlayerAttackNPC * not = reinterpret_cast<Notify_World_To_NPC_PlayerAttackNPC*>(packet);

				if (npcs.count(not->npc_id) > 0)
				{
					Lock();
					NonPlayer *npc = npcs[not->npc_id];
					
					if (npc->GetCurrentState() == ObjectState::Idle)
					{
						npc->SetCurrentState(ObjectState::Battle);
						npc->SetAttackTarget(not->attacker_id);
					}

					unsigned int hp = npc->GetHP();
					hp -= not->damage;

					if (hp  <= 0)
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
						Notify_NPC_To_World_NPCDamaged damaged_notify;
						damaged_notify.gained_damage = not->damage;
						damaged_notify.npc_hp = npc->GetHP();
						damaged_notify.npc_id = npc->GetId();

						Send(id, reinterpret_cast<unsigned char*>(&damaged_notify));
						npc->SetHP(hp);
					}

					Unlock();
				}

				break;
			}
			case ID_Notify_World_To_NPC_PlayerMove:
			{
				Notify_World_To_NPC_PlayerMove *not = reinterpret_cast<Notify_World_To_NPC_PlayerMove*>(packet);

				if (players.count(not->player_id) > 0)
				{
					players[not->player_id]->SetX(not->x);
					players[not->player_id]->SetY(not->y);
				}
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
