#pragma once

#include <list>
#include <unordered_map>

#include "Object.h"
#include "Faction.h"
#include "Quest.h"

class Player : public Object
{
	unsigned int socket_id;

	unsigned int level;
	unsigned long exp;

	unsigned int curr_hp;
	unsigned int max_hp;

	unsigned int base_damage;

	std::unordered_map<unsigned int, Faction> factions;
	std::unordered_map<unsigned int, ObjectType> view_list;
	std::list<Quest> quests;

public:
	Player(unsigned int _uid) : Object(_uid, ObjectType::Player) {}
	~Player();

	void SetSocketId(unsigned int  _sockid) { socket_id = _sockid; }
	void SetLevel(unsigned int _level) { level = _level; }
	void SetExp(unsigned long _exp) { exp = _exp; }
	void SetHP(unsigned int _hp) { curr_hp = _hp; }
	void SetMaxHp(unsigned int _hp) { max_hp = _hp; }
	void SetBaseDamage(unsigned int _dmg) { base_damage = _dmg; }

	void GainExp(unsigned long _exp);

	const unsigned int GetSocketId() { return socket_id; }
	const unsigned int GetLevel() { return level; }
	const unsigned int GetExp() { return exp; }
	const unsigned int GetHP() { return curr_hp; }
	const unsigned int GetMaxHp() { return max_hp; }
	const unsigned int GetBaseDamage() { return base_damage; }

	const unsigned int GetRealDamage() { return base_damage * level; }
	auto GetViewListBegin() { return view_list.begin(); }
	auto GetViewListEnd() { return view_list.end(); }

	void AddViewList(const unsigned int id, ObjectType type) { view_list.insert(std::make_pair(id, type)); }
	void RemoveViewList(const unsigned int id) { view_list.erase(id); }
	bool isExistViewList(const unsigned int id) { return view_list.count(id) != 0 ? true : false; }

	virtual void Update();
};



