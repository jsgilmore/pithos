using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WoWdar
{
    class WoWObject
    {
        public ulong GUID = 0;
        public String Name = "";
        public short Type = 0;
        public uint BaseAddress = 0;
        public uint ObjectFields = 0;
        public uint health = 0;
        public float X = 0;
        public float Y = 0;
        public float Z = 0;
        public float Rot = 0;
    }
}
