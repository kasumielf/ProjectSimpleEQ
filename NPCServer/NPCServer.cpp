#include "NPCServer.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <png.h>

static int SYSTEM_Set_RespawnPosition(lua_State * l)
{
	NPCServer *cls = (NPCServer*)(lua_touserdata(l, -2));
	int player_id = (int)lua_tonumber(l, -1);
	
	Notify_NPC_To_World_PlayerSetRespawnPoint not;
	not.player_id = player_id;
	cls->SendToInternal("World", reinterpret_cast<unsigned char*>(&not));
	lua_pop(l, 7);
	return 0;
}

static int SYSTEM_Send_Message(lua_State * l)
{
	NPCServer *cls = (NPCServer*)(lua_touserdata(l, -4));
	int npc_id = (int)lua_tonumber(l, -3);
	int player_id = (int)lua_tonumber(l, -2);
	const char* answer = lua_tostring(l, -1);

	Response_NPC_To_World_NPCMessage res;
	res.RESPONSE_ID = player_id;
	res.npc_id = npc_id;

	size_t cs;

	if (answer == nullptr || cls == nullptr)
		return 0;

	mbstowcs_s(&cs, res.message,MAX_CHAT_MESSAGE_LENGTH ,answer, MAX_CHAT_MESSAGE_LENGTH);

	cls->SendToInternal("World", reinterpret_cast<unsigned char*>(&res));
	lua_pop(l, 9);

	return 0;
}

NPCServer::NPCServer(const int capacity, const short port) : BaseServer(capacity, port, true), last_add_npc_id(NPC_START_ID)
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
		unsigned int ally_faction;
		unsigned int hostile_faction;
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
	
		
		elem = node->FirstChildElement("AllyFaction");
		elem->QueryUnsignedText(&ally_faction);
		elem = node->FirstChildElement("HostilFaction");
		elem->QueryUnsignedText(&hostile_faction);

		elem = node->FirstChildElement("RespawnTime");
		elem->QueryDoubleText(&respawn_time);

		tinyxml2::XMLElement *scriptelem = node->FirstChildElement("DescriptScript");

		if (scriptelem != nullptr)
		{
			const char* filename = scriptelem->Attribute("src");
			npc->InitLuaScript(filename);
			lua_register(npc->GetLuaState(), "SYSTEM_Send_Message", SYSTEM_Send_Message);
			lua_register(npc->GetLuaState(), "SYSTEM_Set_RespawnPosition", SYSTEM_Set_RespawnPosition);
		}

		npc->SetLevel(level);
		npc->SetExp(exp);
		npc->SetMaxHp(max_hp);
		npc->SetHP(max_hp);
		npc->SetBaseDamage(base_damage);

		npc->SetAlly(ally_faction);
		npc->SetHostile(hostile_faction);

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

	if (npc->move_paths.size() == 0)
	{
		npc->target_x = player->GetX();
		npc->target_y = player->GetY() - 1;
		npc->MakePath(block[npc->GetY()][npc->GetX()]);
	}
	else
	{
		if (npc->target_x != player->GetX() && npc->target_y != player->GetY())
		{
			npc->target_x = player->GetX();
			npc->target_y = player->GetY() - 1;
			npc->MakePath(block[npc->GetY()][npc->GetX()]);
		}

		if (npc->move_paths.size() > 0)
		{
			Node* target_node = npc->move_paths.front();
			npc->SetX(target_node->x);
			npc->SetY(target_node->y);

			not.npc_id = npc->GetId();
			not.x = npc->GetX();
			not.y = npc->GetY();

			npc->move_paths.pop_front();

			SendToInternal("World", reinterpret_cast<unsigned char*>(&not));
		}
	}
}

void NPCServer::CreateNPC(NonPlayer * npc)
{
	Notify_NPC_To_World_NPCreatedAdd_NPC packet;

	packet.curr_hp = npc->GetHP();
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
			unsigned int my_x = npc->GetX();
			unsigned int my_y = npc->GetY();

			unsigned int target_x = self->players[npc->GetAttackTarget()]->GetX();
			unsigned int target_y = self->players[npc->GetAttackTarget()]->GetY();

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

	self->NPCMoveProcess(npc, self->players[npc->GetAttackTarget()]);
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
		NonPlayer *npc = npcs[not->npc_id];

		Lock();
		if (npc->GetCurrentState() == ObjectState::Idle)
		{
			npc->SetCurrentState(ObjectState::Battle);
			npc->SetAttackTarget(not->attacker_id);

			Event attack_event;
			attack_event.provider = not->npc_id;
			attack_event.event_type = IOCPOpType::OpNPCAttack;

			PushTimerEvent(1000, attack_event);
		}
		Unlock();

		int hp = npc->GetHP();

		hp -= not->damage;

		if (hp <= 0)
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
		Notify_NPC_To_World_NPCDamaged damaged_notify;
		damaged_notify.gained_damage = not->damage;
		damaged_notify.npc_hp = npc->GetHP();
		damaged_notify.npc_id = npc->GetId();

		Send(id, reinterpret_cast<unsigned char*>(&damaged_notify));
		npc->SetHP(hp);
	}
}

void NPCServer::PlayerMove(const int id, Notify_World_To_NPC_PlayerMove * not)
{
	players[not->player_id]->SetX(not->x);
	players[not->player_id]->SetY(not->y);
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

	if (p != nullptr)
	{
		if (IsClosed(np->GetX(), np->GetY(), p->GetX(), p->GetY()))
		{
			char msg[MAX_CHAT_MESSAGE_LENGTH];

			wcstombs(msg, req->message, MAX_CHAT_MESSAGE_LENGTH);

			if (np->HasScript() == true)
				np->DoLuaConversation((void*)this, p->GetId(), msg);
		}
	}
}

bool NPCServer::IsClosed(short from_x, short from_y, short to_x, short to_y)
{
	return (from_x - to_x) * (from_x - to_x) + (from_y - to_y) * (from_y - to_y) <= 2;
}

void abort_(const char * s, ...)
{
	va_list args;
	va_start(args, s);
	vfprintf(stderr, s, args);
	fprintf(stderr, "\n");
	va_end(args);
	abort();
}

std::array<std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT> NPCServer::GetBlockDataFromBitmap(const char * filename)
{
	std::array<std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT> data;

	int x, y;

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep * row_pointers;
	char header[8];    // 8 is the maximum size that can be checked
	png_const_bytep d = 0;

	FILE *fp = fopen(filename, "rb");
	if (!fp)
		abort_("[read_png_file] File %s could not be opened for reading", filename);
	fread(header, 1, 8, fp);
	if (png_sig_cmp((png_const_bytep)header, 0, 8))
		abort_("[read_png_file] File %s is not recognized as a PNG file", filename);


	/* initialize stuff */
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png_ptr)
		abort_("[read_png_file] png_create_read_struct failed");

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		abort_("[read_png_file] png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during init_io");

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	width = png_get_image_width(png_ptr, info_ptr);
	height = png_get_image_height(png_ptr, info_ptr);
	color_type = png_get_color_type(png_ptr, info_ptr);
	bit_depth = png_get_bit_depth(png_ptr, info_ptr);

	number_of_passes = png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr, info_ptr);

	/* read file */
	if (setjmp(png_jmpbuf(png_ptr)))
		abort_("[read_png_file] Error during read_image");

	row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);

	for (y = 0; y<height; y++)
		row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png_ptr, info_ptr));

	png_read_image(png_ptr, row_pointers);

	fclose(fp);

	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			data[j][i] = row_pointers[i][j] == 0 ? true : false;
		}
	}
	return data;
}

void NPCServer::BlockCellInit(std::array< std::array<bool, MAX_WORLD_WIDTH>, MAX_WORLD_HEIGHT > data)
{
	for (int y = 0; y < MAX_WORLD_HEIGHT; ++y)
	{
		for (int x = 0; x < MAX_WORLD_WIDTH; ++x)
		{
			int deg = 0;

			for (int i = 0; i < 8; ++i)
			{
				int v_x = std::round(::sin(deg * 3.14 / 180));
				int v_y = std::round(::cos(deg * 3.14 / 180));

				if (x + v_x >= MAX_WORLD_WIDTH || x + v_x < 0)	v_x = 0;
				if (y + v_y >= MAX_WORLD_HEIGHT || y + v_y < 0)	v_y = 0;

				block[y + v_y][x + v_x][i] = (bool)data[y + v_y][x + v_x];
				deg += 45;
			}
		}
	}

	for (int i = 0; i < MAX_WORLD_WIDTH; i++)
	{
		block[0][i][7] = true;
		block[0][i][0] = true;
		block[0][i][1] = true;

		block[MAX_WORLD_HEIGHT-1][i][5] = true;
		block[MAX_WORLD_HEIGHT-1][i][4] = true;
		block[MAX_WORLD_HEIGHT-1][i][3] = true;
	}

	for (int i = 0; i < MAX_WORLD_HEIGHT; i++)
	{
		block[i][0][7] = true;
		block[i][0][6] = true;
		block[i][0][5] = true;

		block[i][MAX_WORLD_WIDTH-1][1] = true;
		block[i][MAX_WORLD_WIDTH-1][2] = true;
		block[i][MAX_WORLD_WIDTH-1][3] = true;
	}
}


