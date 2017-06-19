using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace Objects
{
    public class NonPlayer : Object
    {
        public uint level;
        public long exp;

        public short hp;
        public uint base_damage;
        public uint faction_group;

        public double respawn_time;
    }
}
