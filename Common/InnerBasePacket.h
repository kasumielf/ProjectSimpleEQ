#pragma once

#include "BasePacket.h"

struct InnerBasePacket : BasePacket
{
	unsigned short RESPONSE_ID;
};