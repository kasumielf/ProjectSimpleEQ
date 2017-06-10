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
        ID_Request_Enter_GameWorld = 6,
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class BasePacket
    {
        public PacketId PACKET_ID;
        public ushort SIZE; 
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class LOGIN : BasePacket
    {
        public LOGIN()
        {
            PACKET_ID = PacketId.ID_LOGIN;
            SIZE = (ushort)Marshal.SizeOf(typeof(LOGIN));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string ID_STR;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class MOVE : BasePacket
    {
        public MOVE()
        {
            PACKET_ID = PacketId.ID_MOVE;
            SIZE = (ushort)Marshal.SizeOf(typeof(MOVE));
        }

        public byte DIR;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class ATTACK : BasePacket
    {
        public ATTACK()
        {
            PACKET_ID = PacketId.ID_ATTACK;
            SIZE = (ushort)Marshal.SizeOf(typeof(ATTACK));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class CHAT : BasePacket
    {
        public CHAT()
        {
            PACKET_ID = PacketId.ID_CHAT;
            SIZE = (ushort)Marshal.SizeOf(typeof(CHAT));
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
            SIZE = (ushort)Marshal.SizeOf(typeof(LOGOUT));
        }

        public uint user_id;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class Request_Enter_GameWorld : BasePacket
    {
        public Request_Enter_GameWorld()
        {
            PACKET_ID = PacketId.ID_Request_Enter_GameWorld;
            SIZE = (ushort)Marshal.SizeOf(typeof(Request_Enter_GameWorld));
        }

        public uint user_uid;
    };

}