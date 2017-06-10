#pragma once
#include <unordered_map>
#include "Object.h"

class Sector
{
private:
	std::unordered_map<unsigned int, Object*> players;
public:
	Sector();
	~Sector();
	void AddPlayer(const unsigned int & id, Object* player);
	void RemovePlayer(const unsigned int& id);
	Object* GetPlayer(const unsigned int& id);
	auto begin() { return players.begin(); }
	auto end() { return players.end(); }
};

