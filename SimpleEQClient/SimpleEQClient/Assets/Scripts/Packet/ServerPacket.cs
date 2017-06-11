using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Runtime.InteropServices;

namespace ServerPacket
{
    public enum PacketId : byte
    {
            ID_LOGIN_OK = 10,
            ID_LOGIN_FAIL = 11,
            ID_POSITION_INFO = 12,
            ID_NOTIFY_CHAT = 13,
            ID_STAT_CHANGE = 14,
            ID_REMOVE_OBJECT = 15,
            ID_ADD_OBJECT = 16,
            ID_CONNECT_SERVER = 17,
            ID_Notify_Player_Enter = 18,
            ID_Notify_Player_Move = 19,
            ID_Notify_Player_Attack_NPC = 71,
            ID_Notify_NPC_Attack_Player = 72,
            ID_Notify_NPC_Damaged = 73,
            ID_Notify_Player_Die = 20,
            ID_Notify_Player_Info = 21,

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
    class LOGIN_OK : BasePacket
    {
        public LOGIN_OK()
        {
            PACKET_ID = PacketId.ID_LOGIN_OK;
            SIZE = (ushort)Marshal.SizeOf(typeof(LOGIN_OK));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string username;
        public uint id;
        public ushort x_pos;
        public ushort y_pos;
        public ushort hp;
        public ushort level;
        public uint exp;
        public ushort max_hp;
        public ushort base_damage;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class LOGIN_FAIL : BasePacket
    {
        public LOGIN_FAIL()
        {
            PACKET_ID = PacketId.ID_LOGIN_FAIL;
            SIZE = (ushort)Marshal.SizeOf(typeof(LOGIN_FAIL));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class POSITION_INFO : BasePacket
    {
        public POSITION_INFO()
        {
            PACKET_ID = PacketId.ID_POSITION_INFO;
            SIZE = (ushort)Marshal.SizeOf(typeof(POSITION_INFO));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class Notify_Chat : BasePacket
    {
        public Notify_Chat()
        {
            PACKET_ID = PacketId.ID_NOTIFY_CHAT;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_Chat));
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
            SIZE = (ushort)Marshal.SizeOf(typeof(STAT_CHANGE));
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
            SIZE = (ushort)Marshal.SizeOf(typeof(REMOVE_OBJECT));
        }
       
        public uint ID;
        public byte TYPE;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class ADD_OBJECT : BasePacket
    {
        public ADD_OBJECT()
        {
            PACKET_ID = PacketId.ID_ADD_OBJECT;
            SIZE = (ushort)Marshal.SizeOf(typeof(ADD_OBJECT));
        }

        public uint ID;
        public byte TYPE;
        public ushort x;
        public ushort y;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string name;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class CONNECT_SERVER : BasePacket
    {
        public CONNECT_SERVER()
        {
            PACKET_ID = PacketId.ID_CONNECT_SERVER;
            SIZE = (ushort)Marshal.SizeOf(typeof(CONNECT_SERVER));
        }

        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 15)]
        public string ip;
        public ushort port;
        public uint user_uid;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class Notifty_Player_Attack_NPC : BasePacket
    {
        public Notifty_Player_Attack_NPC()
        {
            PACKET_ID = PacketId.ID_Notify_Player_Attack_NPC;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notifty_Player_Attack_NPC));
        }

        public uint npc_id;
        public ushort damage;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class Notify_Player_Enter : BasePacket
    {
        Notify_Player_Enter()
        {
            PACKET_ID = PacketId.ID_Notify_Player_Enter;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_Player_Enter));
        }

        public uint user_uid;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 12)]
        public string player_name;
        public ushort x;
        public ushort y;
    }

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class Notify_Player_Move_Position : BasePacket
    {
        Notify_Player_Move_Position()
        {
            PACKET_ID = PacketId.ID_Notify_Player_Move;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_Player_Move_Position));
        }
        public uint id;
        public ushort x;
        public ushort y;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    class Notify_NPC_Damaged : BasePacket
    {
        Notify_NPC_Damaged()
        {
            PACKET_ID = PacketId.ID_Notify_NPC_Damaged;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_NPC_Damaged));
        }

        public uint npc_id;
        public ushort npc_hp;
        public ushort gained_damage;
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class Notify_Player_Die : BasePacket
    {
        Notify_Player_Die()
        {
            PACKET_ID = PacketId.ID_Notify_Player_Die;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_Player_Die));
        }
    };

    [Serializable]
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    class Notify_Player_Info : BasePacket
    {
        Notify_Player_Info()
        {
            PACKET_ID = PacketId.ID_Notify_Player_Info;
            SIZE = (ushort)Marshal.SizeOf(typeof(Notify_Player_Info));
        }

        public ushort HP;
        public ushort LEVEL;
        public ulong EXP;
        public ushort max_hp;
        public ushort base_damage;
    };
}

