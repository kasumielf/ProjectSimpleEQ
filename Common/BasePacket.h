#pragma once

#pragma pack(push, 1)
struct BasePacket
{
    unsigned char PACKET_ID;
    unsigned char SIZE;
};
#pragma pack(pop)
