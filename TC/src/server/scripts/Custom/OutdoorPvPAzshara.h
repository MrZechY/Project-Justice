#ifndef OUTDOOR_PVP_Azshara_
#define OUTDOOR_PVP_Azshara_

#include "OutdoorPvP.h"

#define OutdoorPvPAzsharaBuffZonesNum 6

const uint32 OutdoorPvPAzsharaBuffZones[OutdoorPvPAzsharaBuffZonesNum] = { 3483, 3563, 3562, 3713, 3714, 3836 };

enum OutdoorPvPAzsharaSpells
{
    AlliancePlayerKillReward = 32155,
    HordePlayerKillReward = 32158,
    AllianceBuff = 32071,
    HordeBuff = 32049
};

enum OutdoorPvPAzsharaTowerType
{
    Azshara_TOWER_ALLIANCE = 0,
    Azshara_TOWER_HORDE = 1,
    Azshara_TOWER_NEUTRAL = 2,
    Azshara_TOWER_NUM = 3
};

enum OutDoorPvPAzsharaGraveyards
{
	Azshara_Alliance_Start = 2000,
	Azshara_Alliance_Point = 2001,
	Azshara_Neutral_Point  = 2002,
	Azshara_Horde_Point    = 2003,
	Azshara_Horde_Start    = 2004
};

const uint32 AzsharaGraveyardsId[9] = 
{
	Azshara_Alliance_Start,
	Azshara_Alliance_Point,
	Azshara_Neutral_Point,
	Azshara_Horde_Point,
	Azshara_Horde_Start
};

const float AzsharaSpiritGuidePos[5] =
{
	{1146.45190429688f, 298.784606933594f, 335.141296386719f, 0.0f}, // Alliance Start 
	{465.738708496094f, -3.16659998893738f, 269.935913085938f, 0.0f}, // Alliance Point
	{158.286193847656f, 161.912200927734f, 239.276504516602f, 0.0f}, // Neutral Point
	{-159.66020202636f, 337.959686279297f, 293.386688232422f, 0.0f}, // Horde Point
	{-123.888801574707f, 856.942199707031f, 298.348693847656f, 0.0f} // Horde Start
};

const uint32 Azshara_CREDITMARKER[Azshara_TOWER_NUM] = {19032, 19028, 19029};

const uint32 Azshara_CapturePointEvent_Enter[Azshara_TOWER_NUM] = {11404, 11396, 11388};

const uint32 Azshara_CapturePointEvent_Leave[Azshara_TOWER_NUM] = {11403, 11395, 11387};

enum OutdoorPvPAzsharaWorldStates
{
    Azshara_UI_TOWER_DISPLAY_A = 0x9ba,
    Azshara_UI_TOWER_DISPLAY_H = 0x9b9,

    Azshara_UI_TOWER_COUNT_H = 0x9ae,
    Azshara_UI_TOWER_COUNT_A = 0x9ac,

    Azshara_UI_TOWER_SLIDER_N = 2475,
    Azshara_UI_TOWER_SLIDER_POS = 2474,
    Azshara_UI_TOWER_SLIDER_DISPLAY = 2473
};

const uint32 Azshara_MAP_N[Azshara_TOWER_NUM] = {0x9b5, 0x9b2, 0x9a8};

const uint32 Azshara_MAP_A[Azshara_TOWER_NUM] = {0x9b3, 0x9b0, 0x9a7};

const uint32 Azshara_MAP_H[Azshara_TOWER_NUM] = {0x9b4, 0x9b1, 0x9a6};

const uint32 Azshara_TowerArtKit_A[Azshara_TOWER_NUM] = {65, 62, 67};

const uint32 Azshara_TowerArtKit_H[Azshara_TOWER_NUM] = {64, 61, 68};

const uint32 Azshara_TowerArtKit_N[Azshara_TOWER_NUM] = {66, 63, 69};

const go_type AzsharaCapturePoints[Azshara_TOWER_NUM] =
{
    {600005, 530, -471.462f, 3451.09f, 34.6432f, 0.174533f, 0.0f, 0.0f, 0.087156f, 0.996195f},    // 0 - Alliance
    {600006, 530, -184.889f, 3476.93f, 38.205f, -0.017453f, 0.0f, 0.0f, 0.008727f, -0.999962f},   // 1 - Horde
    {600007, 530, -290.016f, 3702.42f, 56.6729f, 0.034907f, 0.0f, 0.0f, 0.017452f, 0.999848f}     // 2 - Neutral
};

const go_type AzsharaTowerFlags[Azshara_TOWER_NUM] =
{
    {600015, 530, -467.078f, 3528.17f, 64.7121f, 3.14159f, 0.0f, 0.0f, 1.0f, 0.0f},             // 0 alliance
    {600016, 530, -187.887f, 3459.38f, 60.0403f, -3.12414f, 0.0f, 0.0f, 0.999962f, -0.008727f}, // 1 horde
    {600017, 530, -289.610f, 3696.83f, 75.9447f, 3.12414f, 0.0f, 0.0f, 0.999962f, 0.008727f}    // 2 neutrall
};

class OPvPCapturePointAzshara : public OPvPCapturePoint
{
    public:

        OPvPCapturePointAzshara(OutdoorPvP * pvp, OutdoorPvPAzsharaTowerType type);

        void ChangeState();

        void SendChangePhase();

        void FillInitialWorldStates(WorldPacket & data);

        // used when player is activated/inactivated in the area
        bool HandlePlayerEnter(Player* player);
        void HandlePlayerLeave(Player* player);

		struct WorldSafeLocsEntry;

    private:

        OutdoorPvPAzsharaTowerType m_TowerType;
};

class OutdoorPvPAzshara : public OutdoorPvP
{
    friend class OPvPCapturePointAzshara;

    public:

        OutdoorPvPAzshara();

        bool SetupOutdoorPvP();

        void HandlePlayerEnterZone(Player* player, uint32 zone);
        void HandlePlayerLeaveZone(Player* player, uint32 zone);

        bool Update(uint32 diff);

        void FillInitialWorldStates(WorldPacket &data);

        void SendRemoveWorldStates(Player* player);

        void HandleKillImpl(Player* player, Unit* killed);

    private:

        // how many towers are controlled
        uint32 m_AllianceTowersControlled;
        uint32 m_HordeTowersControlled;
};

#endif