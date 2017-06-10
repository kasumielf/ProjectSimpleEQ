using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Collections;
using Objects.Data;

namespace Objects
{
    public class Player : Object
    {
        public uint level;
        public ulong exp;

        public uint max_hp;
        public uint curr_hp;
        public uint base_damage;

        public Dictionary<uint, Faction> factions;
        public List<Quest> quests;
    }
}
