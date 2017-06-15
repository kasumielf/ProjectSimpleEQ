#pragma once

#include <list>
#include <unordered_map>

#include "Object.h"
#include "Faction.h"
#include "Quest.h"

class Player : public Object
{
	unsigned int socket_id;

	unsigned short level;
	unsigned long exp;
	unsigned long require_exp;

	unsigned short curr_hp;
	unsigned short max_hp;

	unsigned short base_damage;

	unsigned short start_x;
	unsigned short start_y;

	std::unordered_map<unsigned int, Faction> factions;
	std::unordered_map<unsigned int, ObjectType> view_list;
	std::list<Quest> quests;

public:
	Player(unsigned int _uid) : Object(_uid, ObjectType::Player) {}
	~Player();

	void SetSocketId(unsigned int  _sockid) { socket_id = _sockid; }
	void SetLevel(unsigned short _level) { level = _level; }
	void SetExp(unsigned long _exp) { exp = _exp; }
	void SetHP(unsigned short _hp) { curr_hp = _hp; }
	void SetMaxHp(unsigned short _hp) { max_hp = _hp; }
	void SetBaseDamage(unsigned short _dmg) { base_damage = _dmg; }
	void SetRequiredExp(unsigned long _exp) { require_exp = _exp; }
	void SetStartX(unsigned long _x) { start_x = _x; }
	void SetStartY(unsigned long _y) { start_y = _y; }

	void GainExp(unsigned long _exp);

	const unsigned int GetSocketId() { return socket_id; }
	const unsigned short GetLevel() { return level; }
	const unsigned long GetExp() { return exp; }
	const unsigned short GetHP() { return curr_hp; }
	const unsigned short GetMaxHp() { return max_hp; }
	const unsigned short GetBaseDamage() { return base_damage; }
	void IncreaseHP(unsigned short hp);
	const unsigned short GetStartX() { return start_x; }
	const unsigned short GetStartY() { return start_y; }


	const unsigned int GetRealDamage() { return base_damage; }
	auto GetViewListBegin() { return view_list.begin(); }
	auto GetViewListEnd() { return view_list.end(); }

	void AddViewList(const unsigned int id, ObjectType type) { view_list.insert(std::make_pair(id, type)); }
	void RemoveViewList(const unsigned int id) { view_list.erase(id); }
	bool isExistViewList(const unsigned int id) { return view_list.count(id) != 0 ? true : false; }
	bool CanLevelUp() { return exp >= require_exp; }

	virtual void Update();
};



