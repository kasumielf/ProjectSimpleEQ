#pragma once

#include "BasePacket.h"

#pragma pack(push, 1)
struct InnerBasePacket : BasePacket
{
    unsigned int RESPONSE_ID;
};
#pragma pack(pop)
