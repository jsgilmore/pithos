using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WoWdar
{
    //offsets for WoW 4.2.2.14545

    public enum ObjectManagerOff : uint
    {
        clientConnection = 0x980558,
        objectManager = 0x463C,
        firstObject = 0xB4,
        nextObject = 0x3C,
        localGuid = 0xB8
    }

    public enum PlayerOff : uint
    {
        LastTargetGUID = 0x00A98C80,   //0xA98C88,
        petGUID = 0xB06C70,
        playerName = 0x980598,
        PlayerComboPoint = 0xA98D25,
        RetrieveCorpseWindow = 0xA98CF8,
    }

    public enum GameObject
    {
        GAMEOBJECT_FIELD_X = 0x110,
        GAMEOBJECT_FIELD_Y = GAMEOBJECT_FIELD_X + 0x4,
        GAMEOBJECT_FIELD_Z = GAMEOBJECT_FIELD_X + 0x8,
        GAMEOBJECT_CREATED_BY = 0x8 * 4,
        objName1 = 0x1CC,
        objName2 = 0xB4,
    }

    public enum CorpsePlayer : uint
    {
        X = 0xA98F9C,
        Y = X + 0x4,
        Z = Y + 0x4,
    }

    public enum ObjectFields : uint
    {
        OBJECT_FIELD_GUID = 0x0,
        OBJECT_FIELD_DATA = 0x8,
        OBJECT_FIELD_TYPE = 0x10,
        OBJECT_FIELD_ENTRY = 0x14,
        OBJECT_FIELD_SCALE_X = 0x18,
        OBJECT_FIELD_PADDING = 0x1C,
    }

    public enum NameOffsets : ulong
    {
        ObjectName1 = 0x1CC,
        ObjectName2 = 0xB4,
        UnitName1 = 0x91C,  //0x142?
        UnitName2 = 0x64,   //0x15E?
        nameStore = 0x959EE0 + 0x8,
        nameMask = 0x24,
        nameBase = 0x1C,
        nameString = 0x20
    }

    public enum ObjectOffsets : uint
    {
        Pos_X = 0x790,
        Pos_Y = Pos_X + 0x4,
        Pos_Z = Pos_X + 0x8,
        Rot = Pos_X + 0x10,
        Guid = 0x30,
        ObjectFields = 0x8,
        GameObjectX = 0x110,                
        GameObjectY = GameObjectX + 0x4,    
        GameObjectZ = GameObjectX + 0x8    
    }

    public enum UnitFields
    {
        UNIT_FIELD_CHARM = 0x20,
        UNIT_FIELD_SUMMON = 0x28,
        UNIT_FIELD_CRITTER = 0x30,
        UNIT_FIELD_CHARMEDBY = 0x38,
        UNIT_FIELD_SUMMONEDBY = 0x40,
        UNIT_FIELD_CREATEDBY = 0x48,
        UNIT_FIELD_TARGET = 0x50,
        UNIT_FIELD_CHANNEL_OBJECT = 0x58,
        UNIT_CHANNEL_SPELL = 0x60,
        UNIT_FIELD_BYTES_0 = 0x64,
        UNIT_FIELD_HEALTH = 0x68,
        UNIT_FIELD_POWER1 = 0x6C,
        UNIT_FIELD_POWER2 = 0x70,
        UNIT_FIELD_POWER3 = 0x74,
        UNIT_FIELD_POWER4 = 0x78,
        UNIT_FIELD_POWER5 = 0x7C,
        UNIT_FIELD_MAXHEALTH = 0x80,
        UNIT_FIELD_MAXPOWER1 = 0x84,
        UNIT_FIELD_MAXPOWER2 = 0x88,
        UNIT_FIELD_MAXPOWER3 = 0x8C,
        UNIT_FIELD_MAXPOWER4 = 0x90,
        UNIT_FIELD_MAXPOWER5 = 0x94,
        UNIT_FIELD_POWER_REGEN_FLAT_MODIFIER = 0x98,
        UNIT_FIELD_POWER_REGEN_INTERRUPTED_FLAT_MODIFIER = 0xAC,
        UNIT_FIELD_LEVEL = 0xC0,
        UNIT_FIELD_FACTIONTEMPLATE = 0xC4,
        UNIT_VIRTUAL_ITEM_SLOT_ID = 0xC8,
        UNIT_FIELD_FLAGS = 0xD4,
        UNIT_FIELD_FLAGS_2 = 0xD8,
        UNIT_FIELD_AURASTATE = 0xDC,
        UNIT_FIELD_BASEATTACKTIME = 0xE0,
        UNIT_FIELD_RANGEDATTACKTIME = 0xE8,
        UNIT_FIELD_BOUNDINGRADIUS = 0xEC,
        UNIT_FIELD_COMBATREACH = 0xF0,
        UNIT_FIELD_DISPLAYID = 0xF4,
        UNIT_FIELD_NATIVEDISPLAYID = 0xF8,
        UNIT_FIELD_MOUNTDISPLAYID = 0xFC,
        UNIT_FIELD_MINDAMAGE = 0x100,
        UNIT_FIELD_MAXDAMAGE = 0x104,
        UNIT_FIELD_MINOFFHANDDAMAGE = 0x108,
        UNIT_FIELD_MAXOFFHANDDAMAGE = 0x10C,
        UNIT_FIELD_BYTES_1 = 0x110,
        UNIT_FIELD_PETNUMBER = 0x114,
        UNIT_FIELD_PET_NAME_TIMESTAMP = 0x118,
        UNIT_FIELD_PETEXPERIENCE = 0x11C,
        UNIT_FIELD_PETNEXTLEVELEXP = 0x120,
        UNIT_DYNAMIC_FLAGS = 0x124,
        UNIT_MOD_CAST_SPEED = 0x128,
        UNIT_MOD_CAST_HASTE = 0x12C,
        UNIT_CREATED_BY_SPELL = 0x130,
        UNIT_NPC_FLAGS = 0x134,
        UNIT_NPC_EMOTESTATE = 0x138,
        UNIT_FIELD_STAT0 = 0x13C,
        UNIT_FIELD_STAT1 = 0x140,
        UNIT_FIELD_STAT2 = 0x144,
        UNIT_FIELD_STAT3 = 0x148,
        UNIT_FIELD_STAT4 = 0x14C,
        UNIT_FIELD_POSSTAT0 = 0x150,
        UNIT_FIELD_POSSTAT1 = 0x154,
        UNIT_FIELD_POSSTAT2 = 0x158,
        UNIT_FIELD_POSSTAT3 = 0x15C,
        UNIT_FIELD_POSSTAT4 = 0x160,
        UNIT_FIELD_NEGSTAT0 = 0x164,
        UNIT_FIELD_NEGSTAT1 = 0x168,
        UNIT_FIELD_NEGSTAT2 = 0x16C,
        UNIT_FIELD_NEGSTAT3 = 0x170,
        UNIT_FIELD_NEGSTAT4 = 0x174,
        UNIT_FIELD_RESISTANCES = 0x178,
        UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE = 0x194,
        UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE = 0x1B0,
        UNIT_FIELD_BASE_MANA = 0x1CC,
        UNIT_FIELD_BASE_HEALTH = 0x1D0,
        UNIT_FIELD_BYTES_2 = 0x1D4,
        UNIT_FIELD_ATTACK_POWER = 0x1D8,
        UNIT_FIELD_ATTACK_POWER_MOD_POS = 0x1DC,
        UNIT_FIELD_ATTACK_POWER_MOD_NEG = 0x1E0,
        UNIT_FIELD_ATTACK_POWER_MULTIPLIER = 0x1E4,
        UNIT_FIELD_RANGED_ATTACK_POWER = 0x1E8,
        UNIT_FIELD_RANGED_ATTACK_POWER_MOD_POS = 0x1EC,
        UNIT_FIELD_RANGED_ATTACK_POWER_MOD_NEG = 0x1F0,
        UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER = 0x1F4,
        UNIT_FIELD_MINRANGEDDAMAGE = 0x1F8,
        UNIT_FIELD_MAXRANGEDDAMAGE = 0x1FC,
        UNIT_FIELD_POWER_COST_MODIFIER = 0x200,
        UNIT_FIELD_POWER_COST_MULTIPLIER = 0x21C,
        UNIT_FIELD_MAXHEALTHMODIFIER = 0x238,
        UNIT_FIELD_HOVERHEIGHT = 0x23C,
        UNIT_FIELD_MAXITEMLEVEL = 0x240,
        UNIT_FIELD_PADDING = 0x244,
    }
}
