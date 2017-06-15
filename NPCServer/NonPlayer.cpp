#include "NonPlayer.h"


NonPlayer::~NonPlayer()
{
}

void NonPlayer::SetRegenTime()
{
	next_regen_time = std::chrono::high_resolution_clock::now() + std::chrono::duration<double, std::milli>(respawn_time);
}

void NonPlayer::Update()
{
}
