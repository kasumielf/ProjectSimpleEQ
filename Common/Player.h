#pragma once

#include <list>
#include <unordered_map>

#include "Object.h"
#include "Faction.h"
#include "Quest.h"

class Player : public Object
{
	unsigned int level;
	unsigned long exp;

	unsigned int hp;
	unsigned int base_damage;

	std::unordered_map<unsigned int, Faction> factions;
	std::list<Quest> quests;

public:
	Player(unsigned int _uid) : Object(_uid, ObjectType::Player) {}
	~Player();

	void SetLevel(unsigned int _level) { level = _level; }
	void SetExp(unsigned long _exp) { exp = _exp; }
	void SetHP(unsigned int _hp) { hp = _hp; }
	void SetBaseDamage(unsigned int _dmg) { base_damage = _dmg; }

	const unsigned int GetLevel() { return level; }
	const unsigned int GetExp() { return exp; }
	const unsigned int GetHP() { return hp; }
	const unsigned int GetRealDamage();

	virtual void Update();
};



