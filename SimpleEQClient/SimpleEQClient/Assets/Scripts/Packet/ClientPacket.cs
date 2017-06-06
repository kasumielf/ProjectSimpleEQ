using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ClientPacket
{
    enum PacketId : byte
    {
        ID_LOGIN = 1,
        ID_MOVE = 2,
        ID_ATTACK = 3,
        ID_CHAT = 4,
        ID_LOGOUT = 5,
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class BasePacket
    {
        public PacketId PACKET_ID;
        public short SIZE; 
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class LOGIN : BasePacket
    {
        public LOGIN()
        {
            PACKET_ID = PacketId.ID_LOGIN;
            SIZE = (byte)Marshal.SizeOf(typeof(LOGIN));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string id_srt;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class MOVE : BasePacket
    {
        public MOVE()
        {
            PACKET_ID = PacketId.ID_MOVE;
            SIZE = (byte)Marshal.SizeOf(typeof(MOVE));
        }

        public byte direction;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class ATTACK : BasePacket
    {
        public ATTACK()
        {
            PACKET_ID = PacketId.ID_ATTACK;
            SIZE = (byte)Marshal.SizeOf(typeof(ATTACK));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class CHAT : BasePacket
    {
        public CHAT()
        {
            PACKET_ID = PacketId.ID_CHAT;
            SIZE = (byte)Marshal.SizeOf(typeof(CHAT));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
        public string CHAT_STR;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class LOGOUT : BasePacket
    {
        public LOGOUT()
        {
            PACKET_ID = PacketId.ID_LOGOUT;
            SIZE = (byte)Marshal.SizeOf(typeof(LOGOUT));
        }
    };
}