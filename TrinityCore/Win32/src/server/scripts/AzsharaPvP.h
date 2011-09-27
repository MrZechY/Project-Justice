#ifndef ICECROWN_CITADEL_H_
#define ICECROWN_CITADEL_H_

#include "Creature.h"

#define AzsharaScriptName "AzsharaPvP"

extern Position const RangedPos;
extern Position const MeleePos;
extern Position const CasterPos;

enum TeleporterSpells
{
	TeleSpell = 70781
};

enum CreatureIds
{
	Tower_Elite_H    = 300001, // Melee
	Tower_Elite_A    = 300004,
	Graveyard_Elite_H  = 300002, // Ranged
	Graveyard_Elite_A  = 300005,
	Gate_Elite_H       = 300003, // Caster
	Gate_Elite_A       = 300006
};

enum GameObjectIds
{
	ALLIANCE_BANNER  = 770001,
	HORDE_BANNER     = 770002,
	NEUTRAL_BANNER_A = 770003,
	NEUTRAL_BANNER_H = 770004,

	ALLIANCE_AURA    = 770005,
	HORDE_AURA       = 770006,
	NEUTRAL_AURA     = 770007
};

const float Nodes_Position[5][4] = 
{
	{652.742004f, 113.125000f, 298.562988f, 6.279880f}, // Alli gate
	{441.865326f, 145.802536f, 305.963654f, 3.898160f}, // Alli point
	{160.116699f, 155.424240f, 239.206512f, 5.184363f}, // Neutral point
	{152.231125f, 267.817352f, 295.855682f, 2.687708f}, // Horde point
	{24.245409f, 452.632629f, 325.423889f, 4.638653f} // Horde gate
};

#endif