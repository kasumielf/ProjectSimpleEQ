#include "Player.h"

Player::~Player()
{
	view_list.clear();
}

void Player::GainExp(unsigned long _exp)
{
	exp += _exp;
}

void Player::IncreaseHP(unsigned short hp)
{
	if (curr_hp + hp > max_hp)
		curr_hp = max_hp;
	else
		curr_hp += hp;
}

void Player::Update()
{
}
