#include "Player.h"

Player::~Player()
{
}

void Player::GainExp(unsigned long _exp)
{
	exp += _exp;

	// ToDo : �ӽ� �ڵ�
	if (exp >= 500)
	{
		level += 1;
	}
}

void Player::Update()
{
}
