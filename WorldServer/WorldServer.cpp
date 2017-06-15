#include <iostream>

#include "WorldServer.h"
#include "tinyxml2/tinyxml2.h"

WorldServer::WorldServer(const int capacity, const short port) : BaseServer(capacity, port)
{
	world = new World();

	for (int i = 0; i < capacity; ++i)
	{
		players.push_back(nullptr);
	}

	AttachIOCPEvent(IOCPOpType::OpPlayerAttack, std::bind(&WorldServer::PlayerAttackUpdate, this, std::placeholders::_1, this));
	AttachIOCPEvent(IOCPOpType::OpPlayerUpdate, std::bind(&WorldServer::PlayerUpdate, this, std::placeholders::_1, this));
	AttachIOCPEvent(IOCPOpType::OpPlayerDBSave, std::bind(&WorldServer::PlayerDBSave, this, std::placeholders::_1, this));

	InitStatusTable();
}

WorldServer::~WorldServer()
{
	world->Destroy();
	world = nullptr;
	delete world;
}

void WorldServer::ProcessPacket(const int id, unsigned char * packet)
{
	unsigned char packet_id = packet[0];

	if (packet_id > 0)
	{
		switch (packet_id)
		{
			case ID_Request_Auth_To_World_AllocateUser:
			{
				AllocateUser(id, reinterpret_cast<Request_Auth_To_World_AllocateUser*>(packet));
				break;
			}
			case ID_Response_DB_To_World_GetUserStatus:
			{
				GetUserState(id, reinterpret_cast<Response_DB_To_World_GetUserStatus*>(packet));
				break;
			}
			case ID_Request_Enter_GameWorld:
			{
				EnterGameWorld(id, reinterpret_cast<Request_Enter_GameWorld*>(packet));
				break;
			}
			case ID_MOVE:
			{
				Move(id, reinterpret_cast<MOVE*>(packet));
				break;
			}
			case ID_ATTACK:
			{
				Attack(id, nullptr);
				break;
			}
			case ID_CHAT:
			{
				SendChatMessage(id, reinterpret_cast<CHAT*>(packet));
				break;
			}
			case ID_LOGOUT:
			{
				Logout(id, reinterpret_cast<LOGOUT*>(packet));
				break;
			}
			case ID_Notify_NPC_To_World_NPCreated:
			{
				NPCCreated(id, reinterpret_cast<Notify_NPC_To_World_NPCreatedAdd_NPC*>(packet));
				break;
			}
			case ID_Notify_NPC_To_World_NPCMove:
			{
				break;
			}
			case ID_Notify_NPC_To_World_NPCDieFromPlayer:
			{
				NPCDieFromPlayer(id, reinterpret_cast<Notify_NPC_To_World_NPCDieFromPlayer*>(packet));
				break;
			}
			case ID_Notify_NPC_To_World_NPCDamaged:
			{
				NPCDamaged(id, reinterpret_cast<Notify_NPC_To_World_NPCDamaged*>(packet));
				break;
			}
			case ID_Notify_NPC_To_World_NPCAttackPlayer:
			{
				NPCAttackPlayer(id, reinterpret_cast<Notify_NPC_To_World_NPCAttackPlayer*>(packet));
				break;
			}
			case ID_Response_NPC_To_World_NPCMessage:
			{
				NotifyNPCMesage(id, reinterpret_cast<Response_NPC_To_World_NPCMessage*>(packet));
				break;
			}
		}
	}
}

void WorldServer::Logging(const wchar_t * msg, ...)
{
	va_list ap;
	va_start(ap, msg);
	vwprintf_s(msg, ap);
	va_end(ap);
	std::cout << std::endl;
}

void WorldServer::OnCloseSocket(const int id)
{
	Logging(L"Player %d is closed", id);

	Lock();
	world->DeleteObject(players[id]);
	players[id] = nullptr;
	Unlock();
}

void WorldServer::PlayerAttackUpdate(unsigned int id, WorldServer * self)
{
	if (self->players[id] != nullptr)
	{
		Player *p = self->players[id];

		if (p == nullptr || p->GetCurrentState() != ObjectState::Battle)
			return;

		if (p->GetCurrentState() == ObjectState::Battle)
		{
			unsigned int attack_list[8];

			int attack_list_count = 0;

			unsigned short my_x = p->GetX();
			unsigned short my_y = p->GetY();

			//ToDo : 월드의 경계(최대, 최소) 처리 필요
			for (int i = my_y - 1; i <= my_y + 1; ++i)
			{
				for (int j = my_x - 1; j <= my_x + 1; ++j)
				{
					Object *o = self->world->GetObject(j, i);

					if (o != nullptr && o->GetType() == ObjectType::NonPlayer)
					{
						attack_list[attack_list_count++] = self->world->GetObject(j, i)->GetId();
					}
				}
			}

			if (attack_list_count == 0)
			{
				Notify_Player_Attack_NPC packet;
				packet.damage = p->GetBaseDamage();

				packet.npc_id = 0;
				Send(id, reinterpret_cast<unsigned char*>(&packet));
			}
			else
			{
				for (int i = 0; i < attack_list_count; ++i)
				{
					Notify_Player_Attack_NPC packet;
					packet.damage = p->GetBaseDamage();

					packet.npc_id = attack_list[i];
					Send(id, reinterpret_cast<unsigned char*>(&packet));

					Notify_World_To_NPC_PlayerAttackNPC attack_packet;
					attack_packet.attacker_id = p->GetId();
					attack_packet.damage = p->GetRealDamage();
					attack_packet.npc_id = attack_list[i];

					SendToInternal("NPC", reinterpret_cast<unsigned char*>(&attack_packet));
				}
			}

			Event attack_event;
			attack_event.provider = id;
			attack_event.event_type = IOCPOpType::OpPlayerAttack;

			self->PushTimerEvent(1000, attack_event);
		}

	}
}

void WorldServer::PlayerDBSave(unsigned int id, WorldServer * self)
{
	if (self->players[id] == nullptr)
		return;

	Player *p = self->players[id];

	Request_World_To_DB_UpdateUserStatus update_packet;

	update_packet.user_uid = p->GetId();
	update_packet.RESPONSE_ID = id;
	update_packet.level = p->GetLevel();
	update_packet.exp = p->GetExp();
	update_packet.hp = p->GetHP();
	update_packet.max_hp = p->GetMaxHp();
	update_packet.x = p->GetX();
	update_packet.y = p->GetY();

	SendToInternal("World", reinterpret_cast<unsigned char*>(&update_packet));

	Event player_db_save_event;
	player_db_save_event.event_type = IOCPOpType::OpPlayerDBSave;
	player_db_save_event.provider = id;

	self->PushTimerEvent(60000, player_db_save_event);
}

void WorldServer::PlayerUpdate(unsigned int id, WorldServer * self)
{
	if (self->players[id] == nullptr)
		return;

	Player *p = self->players[id];

	if (p->GetHP() < p->GetMaxHp())
	{
		int val = p->GetMaxHp() * 0.1;
		p->IncreaseHP(val);

		Notify_Player_HPRegen not;
		not.curr_hp = p->GetHP();

		Send(id, reinterpret_cast<unsigned char*>(&not));
	}

	Event player_update_event;
	player_update_event.event_type = IOCPOpType::OpPlayerUpdate;
	player_update_event.provider = id;

	self->PushTimerEvent(1000, player_update_event);
}

bool WorldServer::IsClosed(short from_x, short from_y, short to_x, short to_y)
{
	return (from_x - to_x) * (from_x - to_x) + (from_y - to_y) * (from_y - to_y) <= MAX_SIGHT_RANGE * MAX_SIGHT_RANGE;
}

void WorldServer::InitStatusTable()
{
		tinyxml2::XMLDocument doc;

		int error = doc.LoadFile("StatusTable.xml");

		if (error == 0)
		{
			for (tinyxml2::XMLElement* child = doc.FirstChildElement("Status"); child != NULL; child = child->NextSiblingElement())
			{
				Status status;

				status.level = atoi(child->Attribute("level"));
				status.required_exp = atol(child->Attribute("required_exp"));
				status.base_damage = atoi(child->Attribute("base_damage"));
				status.max_hp = atoi(child->Attribute("max_hp"));

				level_data.push_back(status);
			}
		}
		else
		{
			Logging(L"tinyXML2 error : %d", error);
		}

}

void WorldServer::MovePlayer(int id, Player * p)
{
	Notify_Player_Move_Position notify;
	notify.id = p->GetId();
	notify.x = p->GetX();
	notify.y = p->GetY();

	short my_x = p->GetX();
	short my_y = p->GetY();

	world->SetSector(p, my_x, my_y);

	short sector_x = p->getCurrSectorX();
	short sector_y = p->getCurrSectorY();

	int start_i = 0, start_j = 0;

	if (my_x % MAX_SECTOR_WIDTH < MAX_SECTOR_WIDTH / 2)
		start_j = -1;


	if (my_y % MAX_SECTOR_HEIGHT < MAX_SECTOR_HEIGHT / 2)
		start_i = -1;

	int dest_i = 1 + start_i, dest_j = 1 + start_j;

	std::unordered_map<unsigned int, ObjectType> new_view_list;
	std::unordered_map<unsigned int, ObjectType> del_view_list;

	for (int i = start_i; i <= dest_i; i++)
	{
		for (int j = start_j; j <= dest_j; j++)
		{
			int sec_x = sector_x + j;
			int sec_y = sector_y + i;

			if (sec_x < 0)
				sec_x = 0;

			if (sec_y < 0)
				sec_y = 0;

			if (sec_x >= MAX_SECTOR_WIDTH)
				sec_x = MAX_SECTOR_WIDTH - 1;

			if (sec_y >= MAX_SECTOR_HEIGHT)
				sec_y = MAX_SECTOR_HEIGHT - 1;

			auto iter_b = world->GetPlayerBegin(sec_x, sec_y);
			auto iter_e = world->GetPlayerEnd(sec_x, sec_y);

			for (; iter_b != iter_e; ++iter_b)
			{
				if ((*iter_b).second != nullptr && (*iter_b).second->GetId() != id)
				{
					short x = (*iter_b).second->GetX();
					short y = (*iter_b).second->GetY();

					if (IsClosed(my_x, my_y, x, y))
					{
						OverlappedEx overlapped;
						overlapped.optype = IOCPOpType::OpPlayerMove;

						new_view_list[(*iter_b).second->GetId()] = (*iter_b).second->GetType();

						if ((*iter_b).second->GetType() != ObjectType::NonPlayer)
						{
							Send(socketIds[(*iter_b).second->GetId()], reinterpret_cast<unsigned char*>(&notify));
						}
						else
						{
							overlapped.caller_id = id;
							//PostQueuedCompletionStatus(m_iocp_handle, 1, socketIds[(*iter_b).second->GetId()], reinterpret_cast<LPOVERLAPPED>(&overlapped));
						}
					}
					else
					{
						del_view_list.insert(std::make_pair((*iter_b).second->GetId(), (*iter_b).second->GetType()));
					}
				}
			}
		}
	}

	for (auto target : new_view_list)
	{
		if (p->isExistViewList(target.first) == false && target.first != p->GetId())
		{
			int target_id = target.first;
			ObjectType target_type = target.second;

			ADD_OBJECT addNotify;
			addNotify.x = p->GetX();
			addNotify.y = p->GetY();
			wcscpy_s(addNotify.name, p->GetName());

			if (target_type == ObjectType::Player)
			{
				addNotify.ID = target_id;
				p->AddViewList(target_id, target_type);
				p->AddViewList(id, ObjectType::Player);
				Send(id, reinterpret_cast<unsigned char*>(&addNotify));

				addNotify.ID = id;
				wcscpy_s(addNotify.name, p->GetName());
				Send(socketIds[target_id], reinterpret_cast<unsigned char*>(&addNotify));
			}
			else
			{
				addNotify.ID = target_id;
				addNotify.x = world->GetObjectById(target_id)->GetX();
				addNotify.y = world->GetObjectById(target_id)->GetY();
				wcscpy_s(addNotify.name, world->GetObjectById(target_id)->GetName());

				p->AddViewList(target_id, target_type);
				Send(id, reinterpret_cast<unsigned char*>(&addNotify));
			}
		}
	}

	for (auto target : del_view_list)
	{
		if (p->isExistViewList(target.first) == true)
		{
			int target_id = target.first;
			ObjectType target_type = target.second;

			REMOVE_OBJECT dspNotify;

			dspNotify.ID = target_id;

			if (target_type == ObjectType::Player)
			{
				p->RemoveViewList(target_id);
				p->RemoveViewList(id);
				Send(id, reinterpret_cast<unsigned char*>(&dspNotify));
				dspNotify.ID = id;
				Send(socketIds[target_id], reinterpret_cast<unsigned char*>(&dspNotify));
			}
			else
			{
				dspNotify.ID = target_id;
				p->RemoveViewList(target_id);
				Send(id, reinterpret_cast<unsigned char*>(&dspNotify));
			}
		}
	}
}

void WorldServer::AllocateUser(const int id, Request_Auth_To_World_AllocateUser * req)
{
	if (world->GetObjectById(req->user_uid) == nullptr)
	{
		Request_World_To_DB_GetUserStatus user_req;
		user_req.RESPONSE_ID = id;
		user_req.user_uid = req->user_uid;
		user_req.client_id = req->RESPONSE_ID;

		Logging(L"Find User(uid %d) from DB", req->user_uid);
		SendToInternal("DB", reinterpret_cast<unsigned char*>(&user_req));
	}
	else
	{
		Response_World_To_Auth_AllocateUser res;
		res.RESPONSE_ID = req->RESPONSE_ID;
		res.client_id = req->RESPONSE_ID;
		res.success = false;


		Logging(L"User(uid %d) is failed allocated in Game World", req->user_uid);
		Send(id, reinterpret_cast<unsigned char*>(&res));
	}

}

void WorldServer::GetUserState(const int id, Response_DB_To_World_GetUserStatus * res)
{
	Response_World_To_Auth_AllocateUser auth_res;

	auth_res.success = res->user_uid > 0;

	if (auth_res.success)
	{
		Player *p = new Player(res->user_uid);
		Status levelData = level_data[res->level];

		p->SetName(res->username);
		p->SetX(res->x);
		p->SetY(res->y);
		p->SetHP(res->hp);
		p->SetMaxHp(res->max_hp);
		p->SetLevel(res->level);
		p->SetExp(res->exp);
		p->SetStartX(res->start_x);
		p->SetStartY(res->start_x);
		p->SetBaseDamage(levelData.base_damage);
		world->CreateObject(p);
		world->AddObject(p, p->GetX(), p->GetY());
		p->SetCurrentState(ObjectState::Idle);
	}

	auth_res.RESPONSE_ID = res->RESPONSE_ID;
	auth_res.client_id = res->client_id;
	auth_res.user_uid = res->user_uid;
	Send(res->RESPONSE_ID, reinterpret_cast<unsigned char*>(&auth_res));

}

void WorldServer::EnterGameWorld(const int id, Request_Enter_GameWorld * req)
{
	Player *p = dynamic_cast<Player*>(world->GetObjectById(req->user_uid));

	if (p != nullptr)
	{
		LOGIN_OK res;

		res.base_damage = p->GetBaseDamage();
		res.EXP = p->GetExp();
		res.HP = p->GetHP();
		res.ID = p->GetId();
		res.LEVEL = p->GetLevel();
		res.max_hp = p->GetMaxHp();
		res.X_POS = p->GetX();
		res.Y_POS = p->GetY();
		wcscpy_s(res.username, p->GetName());

		socketIds[p->GetId()] = id;

		players[id] = p;

		Notify_World_To_NPC_PlayerEntered enter_packet;
		enter_packet.player_id = p->GetId();
		enter_packet.x = p->GetX();
		enter_packet.y = p->GetY();

		SendToInternal("NPC", reinterpret_cast<unsigned char*>(&enter_packet));

		Event player_update_event;
		player_update_event.event_type = IOCPOpType::OpPlayerUpdate;
		player_update_event.provider = id;

		PushTimerEvent(1000, player_update_event);

		Event player_db_save_event;
		player_db_save_event.event_type = IOCPOpType::OpPlayerDBSave;
		player_db_save_event.provider = id;

		PushTimerEvent(60000, player_db_save_event);

		Send(id, reinterpret_cast<unsigned char*>(&res));

		world->SetSector(p, p->GetX(), p->GetY());

		short sector_x = p->getCurrSectorX();
		short sector_y = p->getCurrSectorY();

		short start_i = 0, start_j = 0;

		const short my_x = p->GetX();
		const short my_y = p->GetY();

		Notify_Player_Enter notify;

		notify.user_uid = id;
		notify.x = my_x;
		notify.y = my_y;
		wcscpy_s(notify.player_name, p->GetName());

		if (my_x % MAX_SECTOR_WIDTH < MAX_SECTOR_WIDTH / 2)
			start_j = -1;

		if (my_y % MAX_SECTOR_HEIGHT < MAX_SECTOR_HEIGHT / 2)
			start_i = -1;

		short dest_i = start_i;
		short dest_j = start_j;

		dest_i++;
		dest_j++;

		for (int i = start_i; i <= dest_i; i++)
		{
			for (int j = start_j; j <= dest_j; j++)
			{
				int sec_x = sector_x + j;
				int sec_y = sector_y + i;

				if (sec_x < 0)
					sec_x = 0;

				if (sec_y < 0)
					sec_y = 0;

				auto iter_b = world->GetPlayerBegin(sec_x, sec_y);
				auto iter_e = world->GetPlayerEnd(sec_x, sec_y);

				for (; iter_b != iter_e; ++iter_b)
				{
					short target_id = (*iter_b).second->GetId();
					ObjectType target_type = (*iter_b).second->GetType();

					if (target_id != p->GetId())
					{
						short x = (*iter_b).second->GetX();
						short y = (*iter_b).second->GetY();

						if (IsClosed(my_x, my_y, x, y))
						{
							if (target_type != ObjectType::NonPlayer && target_id != p->GetId())
								Send(socketIds[target_id], reinterpret_cast<unsigned char*>(&notify));

							p->AddViewList(target_id, target_type);

							ADD_OBJECT addNotify;

							Object *o = (*iter_b).second;
							addNotify.ID = o->GetId();
							addNotify.x = o->GetX();
							addNotify.y = o->GetY();
							wcscpy_s(addNotify.name, o->GetName());

							Send(id, reinterpret_cast<unsigned char*>(&addNotify));
						}
					}
				}
			}
		}
	}

}

void WorldServer::Move(const int id, MOVE * req)
{
	Player* myPlayer = players[id];
	world->MoveObject(myPlayer, req->DIR);
	MovePlayer(id, myPlayer);
}

void WorldServer::Attack(const int id, ATTACK * req)
{
	if (players[id]->GetCurrentState() == ObjectState::Battle)
	{
		players[id]->SetCurrentState(ObjectState::Idle);
	}
	else
	{
		players[id]->SetCurrentState(ObjectState::Battle);

		Event attack_event;
		attack_event.provider = id;
		attack_event.event_type = IOCPOpType::OpPlayerAttack;

		PushTimerEvent(1000, attack_event);
	}

}

void WorldServer::Logout(const int id, LOGOUT * req)
{
	Object *o = players[id];

	if (o != nullptr)
	{
		Logging(L"Player %d is closed", id);
		Lock();
		world->DeleteObject(players[id]);
		players[id] = nullptr;
		Unlock();
	}
	else
	{
		CloseSocket(id);
	}

	Logging(L"Player %d Exit", id);

}

void WorldServer::NPCCreated(const int id, Notify_NPC_To_World_NPCreatedAdd_NPC * not)
{
	// World서버에선 몬스터 상세 정보 상관 없음
	NonPlayer *npc = new NonPlayer(not->id, 0);
	npc->SetLevel(not->level);
	npc->SetFactionGroup(not->faction_group);
	npc->SetX(not->x);
	npc->SetY(not->y);
	npc->SetHP(not->curr_hp);
	npc->SetMaxHp(not->max_hp);
	npc->SetName(not->name);

	Logging(L"[%d] NPC Create %ws\t\t\t%ws mob id is %d", id, npc->GetName(), not->name, npc->GetId());
	world->CreateObject(npc);
	world->AddObject(npc, npc->GetX(), npc->GetY());
	world->SetSector(npc, npc->GetX(), npc->GetY());

	ADD_OBJECT addNotify;
	addNotify.ID = npc->GetId();
	addNotify.TYPE = (char)npc->GetType();
	addNotify.x = npc->GetX();
	addNotify.y = npc->GetY();
	wcscpy_s(addNotify.name, npc->GetName());

	auto iter_b = world->GetPlayerBegin(npc->getCurrSectorX(), npc->getCurrSectorY());
	auto iter_e = world->GetPlayerEnd(npc->getCurrSectorX(), npc->getCurrSectorY());

	unsigned char* pk = reinterpret_cast<unsigned char*>(&addNotify);

	for (; iter_b != iter_e; ++iter_b)
	{
		if ((*iter_b).second->GetType() == ObjectType::Player)
			Send(socketIds[(*iter_b).second->GetId()], pk);
	}
}

void WorldServer::NPCDieFromPlayer(const int id, Notify_NPC_To_World_NPCDieFromPlayer * not)
{
	REMOVE_OBJECT removePacket;
	removePacket.ID = not->npc_id;

	Object *o = world->GetObjectById(not->npc_id);

	auto iter_b = world->GetPlayerBegin(o->getCurrSectorX(), o->getCurrSectorY());
	auto iter_e = world->GetPlayerEnd(o->getCurrSectorX(), o->getCurrSectorY());

	unsigned char* pk = reinterpret_cast<unsigned char*>(&removePacket);

	for (; iter_b != iter_e; ++iter_b)
	{
		if ((*iter_b).second->GetType() == ObjectType::Player)
			Send(socketIds[(*iter_b).second->GetId()], pk);
	}

	world->RemoveObject(o->GetX(), o->GetY());
	world->DeleteObject(o);

	Player *p = dynamic_cast<Player*>(world->GetObjectById(not->remover_id));
	p->GainExp(not->gained_exp);
	p->SetCurrentState(ObjectState::Idle);

	if (p->GetExp() >= level_data[p->GetLevel()].required_exp)
	{
		unsigned short level = p->GetLevel();

		if (level_data.size() - 1 > level)
		{
			level++;
			p->SetLevel(level);
			p->SetMaxHp(level_data[level].max_hp);
			p->SetBaseDamage(level_data[level].base_damage);
		}
	}

	Notify_Player_Info infoPacket;

	infoPacket.HP = p->GetHP();
	infoPacket.max_hp = p->GetMaxHp();
	infoPacket.LEVEL = p->GetLevel();
	infoPacket.EXP = p->GetExp();
	infoPacket.base_damage = p->GetBaseDamage();

	Send(socketIds[not->remover_id], reinterpret_cast<unsigned char*>(&infoPacket));

}

void WorldServer::NPCDamaged(const int id, Notify_NPC_To_World_NPCDamaged * not)
{
	Notify_NPC_Damaged damagedPacket;
	damagedPacket.npc_hp = not->npc_hp - not->gained_damage;
	damagedPacket.npc_id = not->npc_id;
	damagedPacket.gained_damage = not->gained_damage;

	Object *o = world->GetObjectById(not->npc_id);

	auto iter_b = world->GetPlayerBegin(o->getCurrSectorX(), o->getCurrSectorY());
	auto iter_e = world->GetPlayerEnd(o->getCurrSectorX(), o->getCurrSectorY());

	unsigned char* pk = reinterpret_cast<unsigned char*>(&damagedPacket);

	for (; iter_b != iter_e; ++iter_b)
	{
		if ((*iter_b).second->GetType() == ObjectType::Player)
			Send(socketIds[(*iter_b).second->GetId()], pk);
	}
}

void WorldServer::NPCAttackPlayer(const int id, Notify_NPC_To_World_NPCAttackPlayer * not)
{
	unsigned short damage = not->damage;
	unsigned int target_id = not->target_id;
	unsigned short npc_id = not->npc_id;

	Object *o = world->GetObjectById(npc_id);
	Player *p = dynamic_cast<Player*>(world->GetObjectById(target_id));

	if (p == nullptr || o == nullptr)
		return;

	bool player_die = false;
	Lock();
	unsigned short hp = p->GetHP();
	hp -= damage;

	if (hp <= damage)
	{
		hp = 1;

		p->SetCurrentState(ObjectState::Die);
		player_die = true;

		p->SetCurrentState(ObjectState::Idle);

		p->SetX(p->GetStartX());
		p->SetY(p->GetStartY());
		world->SetSector(p, p->GetX(), p->GetY());

		MovePlayer(target_id, p);

		Notify_World_To_NPC_NPCStopAttackPlayer not;
		not.npc_id = npc_id;
		not.target_id = target_id;
		SendToInternal("NPC", reinterpret_cast<unsigned char*>(&not));
	}

	p->SetHP(hp);
	Unlock();

	// ToDo : 람다를 활용해서 중복 코드랑 코드 길이를 줄여볼 것.
	auto iter_b = world->GetPlayerBegin(o->getCurrSectorX(), o->getCurrSectorY());
	auto iter_e = world->GetPlayerEnd(o->getCurrSectorX(), o->getCurrSectorY());

	Notify_NPC_Attack_Player attackPacket;
	attackPacket.npc_id = not->npc_id;
	attackPacket.damage = not->damage;

	unsigned char* pk = reinterpret_cast<unsigned char*>(&attackPacket);

	for (; iter_b != iter_e; ++iter_b)
	{
		if ((*iter_b).second->GetType() == ObjectType::Player)
		{
			Send(socketIds[(*iter_b).second->GetId()], pk);

			if (player_die == true)
			{
				REMOVE_OBJECT removePacket;
				removePacket.ID = not->target_id;
				Send(socketIds[(*iter_b).second->GetId()], reinterpret_cast<unsigned char*>(&removePacket));
			}
		}
	}
}

void WorldServer::SendChatMessage(const int id, CHAT * req)
{
	Player *p = players[id];

	if (p == nullptr)
		return;

	short sec_x = p->getCurrSectorX();
	short sec_y = p->getCurrSectorY();

	auto iter_b = world->GetPlayerBegin(sec_x, sec_y);
	auto iter_e = world->GetPlayerEnd(sec_x, sec_y);

	Notify_ChatMessage notMsg;
	Request_World_To_NPC_PlayerChat msg;

	wcscpy_s(notMsg.sender_name, p->GetName());
	wcscpy_s(notMsg.message, req->CHAT_STR);

	msg.sender_id = p->GetId();
	wcscpy_s(msg.message, req->CHAT_STR);

	for (; iter_b != iter_e; ++iter_b)
	{
		unsigned int obj_id = (*iter_b).second->GetId();

		if ((*iter_b).second->GetType() == ObjectType::Player && obj_id != id)
		{
			notMsg.sender_id = p->GetId();
			Send(socketIds[obj_id], reinterpret_cast<unsigned char*>(&notMsg));
		}
		else if ((*iter_b).second->GetType() == ObjectType::NonPlayer)
		{
			msg.target_id = (*iter_b).second->GetId();
			SendToInternal("NPC", reinterpret_cast<unsigned char*>(&msg));
		}
		else
		{
			continue;
		}
	}
}

void WorldServer::NotifyNPCMesage(const int id, Response_NPC_To_World_NPCMessage * res)
{
	Player *p = dynamic_cast<Player*>(world->GetObjectById(res->RESPONSE_ID));

	short sec_x = p->getCurrSectorX();
	short sec_y = p->getCurrSectorY();

	auto iter_b = world->GetPlayerBegin(sec_x, sec_y);
	auto iter_e = world->GetPlayerEnd(sec_x, sec_y);

	Notify_ChatMessage notMsg;

	wcscpy_s(notMsg.sender_name, world->GetObjectById(res->npc_id)->GetName());
	wcscpy_s(notMsg.message, res->message);

	for (; iter_b != iter_e; ++iter_b)
	{
		if ((*iter_b).second->GetType() == ObjectType::Player)
		{
			notMsg.sender_id = p->GetId();
			Send(socketIds[(*iter_b).second->GetId()], reinterpret_cast<unsigned char*>(&notMsg));
		}
	}

}

