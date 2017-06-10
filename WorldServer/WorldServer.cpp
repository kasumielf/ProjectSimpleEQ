#include <iostream>

#include "WorldServer.h"
#include "../Common/ClientPacket.h"
#include "../Common/ServerPacket.h"
#include "../Common/InnerRequestPacket.h"
#include "../Common/InnerResponsePacket.h"
#include "../Common/InnerNotifyPacket.h"

WorldServer::WorldServer(const int capacity, const short port) : BaseServer(capacity, port)
{
	world = new World();

	for (int i = 0; i < capacity; ++i)
	{
		objects.push_back(nullptr);
	}

	AttachIOCPEvent(IOCPOpType::OpPlayerAttack, std::bind(&WorldServer::PlayerAttackUpdate, this, std::placeholders::_1, this));
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
				Request_Auth_To_World_AllocateUser *req = reinterpret_cast<Request_Auth_To_World_AllocateUser*>(packet);

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
				break;
			}

			case ID_Response_DB_To_World_GetUserStatus:
			{
				Response_DB_To_World_GetUserStatus *res = reinterpret_cast<Response_DB_To_World_GetUserStatus*>(packet);
				Response_World_To_Auth_AllocateUser auth_res;

				auth_res.success = res->user_uid > 0;

				if (auth_res.success)
				{
					Player *p = new Player(res->user_uid);
					p->SetName(res->username);
					p->SetX(res->x);
					p->SetY(res->y);
					p->SetHP(res->hp);
					p->SetMaxHp(res->level * 1000);
					p->SetLevel(res->level);
					p->SetExp(res->exp);
					p->SetBaseDamage(100);
					world->CreateObject(p);
					world->AddObject(p, p->GetX(), p->GetY());
					p->SetCurrentState(ObjectState::Idle);
				}

				auth_res.RESPONSE_ID = res->RESPONSE_ID;
				auth_res.client_id = res->client_id;
				auth_res.user_uid = res->user_uid;
				Send(res->RESPONSE_ID, reinterpret_cast<unsigned char*>(&auth_res));

				break;
			}

			case ID_Request_Enter_GameWorld:
			{
				Request_Enter_GameWorld *req = reinterpret_cast<Request_Enter_GameWorld*>(packet);
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

					objects[id] = p;
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

								if (target_id != id)
								{
									short x = (*iter_b).second->GetX();
									short y = (*iter_b).second->GetY();

									if (IsClosed(my_x, my_y, x, y))
									{
										if (target_type != ObjectType::NonPlayer)
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
				break;
			}
			case ID_MOVE:
			{
				MOVE *req = reinterpret_cast<MOVE*>(packet);

				Player* myPlayer = dynamic_cast<Player*>(objects[id]);
				world->MoveObject(myPlayer, req->DIR);

				Request_World_To_DB_UpdateUserPotision update_packet;

				update_packet.RESPONSE_ID = id;
				update_packet.user_uid = myPlayer->GetId();
				update_packet.x = myPlayer->GetX();
				update_packet.y = myPlayer->GetY();

				Notify_Player_Move_Position notify;
				notify.id = id;
				notify.x = myPlayer->GetX();
				notify.y = myPlayer->GetY();


				short my_x = myPlayer->GetX();
				short my_y = myPlayer->GetY();

				world->SetSector(myPlayer, my_x, my_y);

				short sector_x = myPlayer->getCurrSectorX();
				short sector_y = myPlayer->getCurrSectorY();

				int start_i = 0, start_j = 0;

				if (my_x % MAX_SECTOR_WIDTH < MAX_SECTOR_WIDTH / 2)
					start_j = -1;


				if (my_y % MAX_SECTOR_HEIGHT < MAX_SECTOR_HEIGHT / 2)
					start_i = -1;

				int dest_i = 1 + start_i, dest_j = 1 + start_j;

				std::unordered_map<short, ObjectType> new_view_list;
				std::unordered_map<short, ObjectType> del_view_list;

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

									new_view_list.insert(std::make_pair((*iter_b).second->GetId(), (*iter_b).second->GetType()));

									if ((*iter_b).second->GetType() != ObjectType::NonPlayer)
									{
										Send(socketIds[(*iter_b).second->GetId()], reinterpret_cast<unsigned char*>(&notify));
									}
									else
									{
										overlapped.caller_id = id;
										PostQueuedCompletionStatus(m_iocp_handle, 0, (*iter_b).second->GetId(), reinterpret_cast<LPOVERLAPPED>(&overlapped));
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
					if (myPlayer->isExistViewList(target.first) == false)
					{
						int target_id = target.first;
						ObjectType target_type = target.second;

						ADD_OBJECT addNotify;

						if (target_type == ObjectType::Player)
						{
							addNotify.ID = target_id;
							addNotify.x = myPlayer->GetX();
							addNotify.y = myPlayer->GetY();

							myPlayer->AddViewList(target_id, target_type);

							myPlayer->AddViewList(id, ObjectType::Player);
							Send(id, reinterpret_cast<unsigned char*>(&addNotify));

							addNotify.ID = id;
							addNotify.x = myPlayer->GetX();
							addNotify.y = myPlayer->GetY();

							Send(socketIds[target_id], reinterpret_cast<unsigned char*>(&addNotify));
						}
						else
						{
							addNotify.ID = target_id;
							addNotify.x = world->GetObjectById(target_id)->GetX();
							addNotify.y = world->GetObjectById(target_id)->GetY();

							myPlayer->AddViewList(target_id, target_type);
							Send(id, reinterpret_cast<unsigned char*>(&addNotify));
						}
					}
				}

				for (auto target : del_view_list)
				{
					if (myPlayer->isExistViewList(target.first) == true)
					{
						int target_id = target.first;
						ObjectType target_type = target.second;

						REMOVE_OBJECT dspNotify;

						dspNotify.ID = target_id;

						if (target_type == ObjectType::Player)
						{
							myPlayer->RemoveViewList(target_id);
							myPlayer->RemoveViewList(id);
							Send(id, reinterpret_cast<unsigned char*>(&dspNotify));
							dspNotify.ID = id;
							Send(socketIds[target_id], reinterpret_cast<unsigned char*>(&dspNotify));
						}
						else
						{
							dspNotify.ID = target_id;
							myPlayer->RemoveViewList(target_id);
							Send(id, reinterpret_cast<unsigned char*>(&dspNotify));
						}
					}
				}

				SendToInternal("DB", reinterpret_cast<unsigned char*>(&update_packet));
				break;
			}
			case ID_ATTACK:
			{
				if (objects[id]->GetCurrentState() == ObjectState::Battle)
				{
					objects[id]->SetCurrentState(ObjectState::Idle);
				}
				else
				{
					objects[id]->SetCurrentState(ObjectState::Battle);

					Event attack_event;
					attack_event.provider = id;
					attack_event.event_type = IOCPOpType::OpPlayerAttack;

					m_timerEvents.Enqueue(1000, attack_event);
				}
				break;
			}

			case ID_LOGOUT:
			{
				LOGOUT *req = reinterpret_cast<LOGOUT*>(packet);

				Object *o = objects[id];

				if (o != nullptr)
				{
					Lock();
					world->DeleteObject(o->GetId());
					Unlock();
				}
				else
				{
					CloseSocket(id);
				}

				Logging(L"Player %d Exit", id);
				break;
			}

			case ID_Notify_NPC_To_World_NPCreated:
			{
				Notify_NPC_To_World_NPCreatedAdd_NPC *not = reinterpret_cast<Notify_NPC_To_World_NPCreatedAdd_NPC*>(packet);

				// World�������� ���� �� ���� ��� ����
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
				break;
			}
			case ID_Notify_NPC_To_World_NPCMove:
			{
				break;
			}
			case ID_Notify_NPC_To_World_NPCDieFromPlayer:
			{
				break;
			}
			case ID_Notify_NPC_To_World_NPCDamaged:
			{
				break;
			}
			case ID_Notify_NPC_To_World_NPCAttackPlayer:
			{

				// ToDo : Broadcast Player is attacked
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
	socketIds[objects[id]->GetId()] = id;
	world->DeleteObject(objects[id]);
	objects[id] = nullptr;
	Unlock();
}

void WorldServer::PlayerAttackUpdate(unsigned int id, WorldServer * self)
{
	Player *p = dynamic_cast<Player*>(self->objects[id]);

	if (p->GetCurrentState() == ObjectState::Battle)
	{
		unsigned int attack_list[8];

		int attack_list_count = 0;

		unsigned short my_x = p->GetX();
		unsigned short my_y = p->GetY();

		//ToDo : ������ ���(�ִ�, �ּ�) ó�� �ʿ�
		for (int i = my_y - 1; i < my_y + 1; ++i)
		{
			for (int j = my_x - 1; j < my_x + 1; ++j)
			{
				Object *o = self->world->GetObject(j, i);

				if (o != nullptr && o->GetType() == ObjectType::NonPlayer)
				{
					attack_list[attack_list_count++]= self->world->GetObject(j, i)->GetId();
				}
			}
		}

		Notify_Player_Attack_NPC packet;
		packet.damage = p->GetBaseDamage();

		if (attack_list_count == 0)
		{
			packet.npc_id = 0;
			Send(id, reinterpret_cast<unsigned char*>(&packet));
		}
		else
		{
			for (int i = 0; i < attack_list_count; ++i)
			{
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

		self->m_timerEvents.Enqueue(1000, attack_event);
	}
}

bool WorldServer::IsClosed(short from_x, short from_y, short to_x, short to_y)
{
	return (from_x - to_x) * (from_x - to_x) + (from_y - to_y) * (from_y - to_y) <= MAX_SIGHT_RANGE * MAX_SIGHT_RANGE;
}
