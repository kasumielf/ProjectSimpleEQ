using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ServerPacket
{
    enum PacketId : byte
    {
            ID_LOGIN_OK = 1,
            ID_LOGIN_FAIL = 2,
            ID_POSITION_INFO = 3,
            ID_NOTIFY_CHAT = 4,
            ID_STAT_CHANGE = 5,
            ID_REMOVE_OBJECT = 6,
            ID_ADD_OBJECT = 7,
            ID_CONNECT_SERVER = 255,
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class BasePacket
    {
        public PacketId PACKET_ID;
        public ushort SIZE; 
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class LOGIN_OK : BasePacket
    {
        public LOGIN_OK()
        {
            PACKET_ID = PacketId.ID_LOGIN_OK;
            SIZE = (byte)Marshal.SizeOf(typeof(LOGIN_OK));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string username;
        public ushort id;
        public ushort x_pos;
        public ushort y_pos;
        public ushort hp;
        public ushort level;
        public ulong exp; 
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class LOGIN_FAIL : BasePacket
    {
        public LOGIN_FAIL()
        {
            PACKET_ID = PacketId.ID_LOGIN_FAIL;
            SIZE = (byte)Marshal.SizeOf(typeof(LOGIN_FAIL));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class POSITION_INFO : BasePacket
    {
        public POSITION_INFO()
        {
            PACKET_ID = PacketId.ID_POSITION_INFO;
            SIZE = (byte)Marshal.SizeOf(typeof(POSITION_INFO));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class Notify_Chat : BasePacket
    {
        public Notify_Chat()
        {
            PACKET_ID = PacketId.ID_NOTIFY_CHAT;
            SIZE = (byte)Marshal.SizeOf(typeof(Notify_Chat));
        }

        public ushort speaker_id;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
        public string CHAT_STR;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class STAT_CHANGE : BasePacket
    {
        public STAT_CHANGE()
        {
            PACKET_ID = PacketId.ID_STAT_CHANGE;
            SIZE = (byte)Marshal.SizeOf(typeof(STAT_CHANGE));
        }

        public ushort HP;
        public ushort LEVEL;
        public ulong EXP;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class REMOVE_OBJECT : BasePacket
    {
        public REMOVE_OBJECT()
        {
            PACKET_ID = PacketId.ID_REMOVE_OBJECT;
            SIZE = (byte)Marshal.SizeOf(typeof(REMOVE_OBJECT));
        }
       
        public ushort ID;
        public byte TYPE;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class ADD_OBJECT : BasePacket
    {
        public ADD_OBJECT()
        {
            PACKET_ID = PacketId.ID_REMOVE_OBJECT;
            SIZE = (byte)Marshal.SizeOf(typeof(ADD_OBJECT));
        }

        public ushort ID;
        public byte TYPE;
        public ushort x;
        public ushort y;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class CONNECT_SERVER : BasePacket
    {
        public CONNECT_SERVER()
        {
            PACKET_ID = PacketId.ID_CONNECT_SERVER;
            SIZE = (byte)Marshal.SizeOf(typeof(CONNECT_SERVER));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
        public string ip;
        public ushort port;
    };
}