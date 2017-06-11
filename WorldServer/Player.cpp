#include "Player.h"

Player::~Player()
{
}

void Player::GainExp(unsigned long _exp)
{
	exp += _exp;

	// ToDo : 임시 코드
	if (exp >= 500)
	{
		level += 1;
	}
}

void Player::Update()
{
}
