#include "Sector.h"
#include <iostream>

Sector::Sector()
{
}

Sector::~Sector()
{
    players.clear();
}

void Sector::AddPlayer(const unsigned int & id, Object * player)
{
    players[id] = player;
}

void Sector::RemovePlayer(const unsigned int & id)
{
    players.erase(players.find(id));
}

Object* Sector::GetPlayer(const unsigned int & id)
{
    return players[id];
}
