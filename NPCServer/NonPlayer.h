#pragma once

#include "Object.h"
extern "C"
{
#include <lua.h>
#include <lualib.h>
}
#include <lua.hpp>

class NonPlayer : public Object
{
private:
	unsigned int level;
	unsigned long exp;

	unsigned int curr_hp;
	unsigned int max_hp;

	unsigned int base_damage;
	unsigned int faction_group;

	double respawn_time;

	unsigned int attack_target;
	lua_State *l;

	unsigned char attack_count;

public:
	NonPlayer(unsigned int _id, double _rtime) : Object(_id, ObjectType::NonPlayer), respawn_time(_rtime) {}
	~NonPlayer();

	void SetLevel(unsigned int _level) { level = _level; }
	void SetExp(unsigned long _exp) { exp = _exp; }
	void SetHP(unsigned int _hp) { curr_hp = _hp; }
	void SetMaxHp(unsigned int _hp) { max_hp = _hp; }

	void SetBaseDamage(unsigned int _dmg) { base_damage = _dmg; }
	void SetFactionGroup(unsigned int _grp) { faction_group = _grp; }
	void SetAttackTarget(unsigned int _target) { attack_target = _target; }
	void SetRespawnTime(double _rt) { respawn_time = _rt; }

	const unsigned int GetLevel() { return level; }
	const unsigned int GetExp() { return exp; }
	const unsigned int GetHP() { return curr_hp; }
	const unsigned int GetMaxHp() { return max_hp; }

	const unsigned int GetRealDamage() { return base_damage; }
	const unsigned int GetFactionGroup() { return faction_group; }
	const double GetRespawnTime() { return respawn_time; }
	const unsigned int GetAttackTarget() { return attack_target; }

	lua_State* GetLuaState() { return l; }
	void InitLuaScript(const char* filename);
	void DoLuaConversation(void* server_ptr, unsigned int player, char* msg);
	virtual void Update();
};

