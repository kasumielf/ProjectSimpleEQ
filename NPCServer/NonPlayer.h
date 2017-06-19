#pragma once

#include <list>
#include "Object.h"
extern "C"
{
#include <lua.h>
#include <lualib.h>
}
#include <lua.hpp>
#include <chrono>
#include "AStar.h"
#include <bitset>

class NonPlayer : public Object
{
private:
	unsigned int level;
	unsigned long exp;

	short curr_hp;
	short max_hp;

	unsigned int base_damage;

	unsigned short ally_faction;
	unsigned short hostile_faction;

	double respawn_time;

	unsigned int attack_target;
	lua_State *l;

	unsigned char attack_count;
	std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<double>> next_regen_time;
	bool has_script = false;

public:
	AStar path_seracher;
	Node* startNode;
	Node* endNode;
	std::list<Node*> move_paths;
	unsigned int target_x;
	unsigned int target_y;

public:
	NonPlayer(unsigned int _id, double _rtime) : Object(_id, ObjectType::NonPlayer), respawn_time(_rtime)
	{
		startNode = new Node();
		endNode = new Node();
	}
	~NonPlayer();

	void SetLevel(unsigned int _level) { level = _level; }
	void SetExp(unsigned long _exp) { exp = _exp; }
	void SetHP(short _hp) { curr_hp = _hp; }
	void SetMaxHp(short _hp) { max_hp = _hp; }

	void SetAlly(unsigned short id) { ally_faction = id; }
	void SetHostile(unsigned short id) { ally_faction = id; }

	void SetBaseDamage(unsigned int _dmg) { base_damage = _dmg; }
	void SetAttackTarget(unsigned int _target) { attack_target = _target; }
	void SetRespawnTime(double _rt) { respawn_time = _rt; }

	const unsigned int GetLevel() { return level; }
	const unsigned int GetExp() { return exp; }
	const short GetHP() { return curr_hp; }
	const short GetMaxHp() { return max_hp; }

	const unsigned int GetRealDamage() { return base_damage; }
	const double GetRespawnTime() { return respawn_time; }
	const unsigned int GetAttackTarget() { return attack_target; }

	bool IsAlly(unsigned short faction_id) { return faction_id == ally_faction; }
	bool IsHostile(unsigned short faction_id) { return faction_id == hostile_faction; }

	bool HasScript() { return has_script; }
	lua_State* GetLuaState() { return l; }
	void InitLuaScript(const char* filename);
	void DoLuaConversation(void* server_ptr, unsigned int player, char* msg);

	void MakePath(std::bitset<8> &blocks);
//	const lua_State* GetLuaState() { return ls; }
	void SetRegenTime();
	auto GetRegenTime() { return next_regen_time; }
	virtual void Update();

};

