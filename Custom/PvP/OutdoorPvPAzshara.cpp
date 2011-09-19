#include "OutdoorPvPAzshara.h"
#include "OutdoorPvP.h"
#include "OutdoorPvPMgr.h"
#include "Player.h"
#include "WorldPacket.h"
#include "World.h"
#include "ObjectMgr.h"
#include "Language.h"
#include "ScriptPCH.h"

#include "BattlegroundMgr.h"
#include "Battleground.h"
#include "Object.h"
#include "Util.h"

const uint32 Azshara_LANG_LOSE_A[Azshara_TOWER_NUM] = {LANG_OPVP_Azshara_LOSE_ALLIANCE_A, LANG_OPVP_Azshara_LOSE_HORDE_A, LANG_OPVP_Azshara_LOSE_NEUTRAL_A};

const uint32 Azshara_LANG_LOSE_H[Azshara_TOWER_NUM] = {LANG_OPVP_Azshara_LOSE_ALLIANCE_H, LANG_OPVP_Azshara_LOSE_HORDE_H, LANG_OPVP_Azshara_LOSE_NEUTRAL_H};

const uint32 Azshara_LANG_CAPTURE_A[Azshara_TOWER_NUM] = {LANG_OPVP_Azshara_CAPTURE_ALLIANCE_A, LANG_OPVP_Azshara_CAPTURE_HORDE_A, LANG_OPVP_Azshara_CAPTURE_NEUTRAL_A};

const uint32 Azshara_LANG_CAPTURE_H[Azshara_TOWER_NUM] = {LANG_OPVP_Azshara_CAPTURE_ALLIANCE_H, LANG_OPVP_Azshara_CAPTURE_HORDE_H, LANG_OPVP_Azshara_CAPTURE_NEUTRAL_H};

OPvPCapturePointAzshara::OPvPCapturePointAzshara(OutdoorPvP *pvp, OutdoorPvPAzsharaTowerType type)
: OPvPCapturePoint(pvp), m_TowerType(type)
{
    SetCapturePointData(AzsharaCapturePoints[type].entry,
        AzsharaCapturePoints[type].map,
        AzsharaCapturePoints[type].x,
        AzsharaCapturePoints[type].y,
        AzsharaCapturePoints[type].z,
        AzsharaCapturePoints[type].o,
        AzsharaCapturePoints[type].rot0,
        AzsharaCapturePoints[type].rot1,
        AzsharaCapturePoints[type].rot2,
        AzsharaCapturePoints[type].rot3);
    AddObject(type,
        AzsharaTowerFlags[type].entry,
        AzsharaTowerFlags[type].map,
        AzsharaTowerFlags[type].x,
        AzsharaTowerFlags[type].y,
        AzsharaTowerFlags[type].z,
        AzsharaTowerFlags[type].o,
        AzsharaTowerFlags[type].rot0,
        AzsharaTowerFlags[type].rot1,
        AzsharaTowerFlags[type].rot2,
        AzsharaTowerFlags[type].rot3);
}

OutdoorPvPAzshara::OutdoorPvPAzshara()
{
    m_TypeId = OUTDOOR_PVP_Azshara;
}

bool OutdoorPvPAzshara::SetupOutdoorPvP()
{
    m_AllianceTowersControlled = 0;
    m_HordeTowersControlled = 0;
    // add the zones affected by the pvp buff
    for (int i = 0; i < OutdoorPvPAzsharaBuffZonesNum; ++i)
        RegisterZone(OutdoorPvPAzsharaBuffZones[i]);

    AddCapturePoint(new OPvPCapturePointAzshara(this, Azshara_TOWER_ALLIANCE));

    AddCapturePoint(new OPvPCapturePointAzshara(this, Azshara_TOWER_HORDE));

    AddCapturePoint(new OPvPCapturePointAzshara(this, Azshara_TOWER_NEUTRAL));

    return true;
}

void OutdoorPvPAzshara::HandlePlayerEnterZone(Player* player, uint32 zone)
{
    // add buffs
    if (player->GetTeam() == ALLIANCE)
    {
        if (m_AllianceTowersControlled >=3)
            player->CastSpell(player, AllianceBuff, true);
    }
    else
    {
        if (m_HordeTowersControlled >=3)
            player->CastSpell(player, HordeBuff, true);
    }
    OutdoorPvP::HandlePlayerEnterZone(player, zone);
}

void OutdoorPvPAzshara::HandlePlayerLeaveZone(Player* player, uint32 zone)
{
    // remove buffs
    if (player->GetTeam() == ALLIANCE)
    {
        player->RemoveAurasDueToSpell(AllianceBuff);
    }
    else
    {
        player->RemoveAurasDueToSpell(HordeBuff);
    }
    OutdoorPvP::HandlePlayerLeaveZone(player, zone);
}

bool OutdoorPvPAzshara::Update(uint32 diff)
{
    bool changed = OutdoorPvP::Update(diff);
    if (changed)
    {
        if (m_AllianceTowersControlled == 3)
            TeamApplyBuff(TEAM_ALLIANCE, AllianceBuff, HordeBuff);
        else if (m_HordeTowersControlled == 3)
            TeamApplyBuff(TEAM_HORDE, HordeBuff, AllianceBuff);
        else
        {
            TeamCastSpell(TEAM_ALLIANCE, -AllianceBuff);
            TeamCastSpell(TEAM_HORDE, -HordeBuff);
        }
        SendUpdateWorldState(Azshara_UI_TOWER_COUNT_A, m_AllianceTowersControlled);
        SendUpdateWorldState(Azshara_UI_TOWER_COUNT_H, m_HordeTowersControlled);
    }
    return changed;
}

void OutdoorPvPAzshara::SendRemoveWorldStates(Player* player)
{
    player->SendUpdateWorldState(Azshara_UI_TOWER_DISPLAY_A, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_DISPLAY_H, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_COUNT_H, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_COUNT_A, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_N, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_POS, 0);
    player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_DISPLAY, 0);
    for (int i = 0; i < Azshara_TOWER_NUM; ++i)
    {
        player->SendUpdateWorldState(Azshara_MAP_N[i], 0);
        player->SendUpdateWorldState(Azshara_MAP_A[i], 0);
        player->SendUpdateWorldState(Azshara_MAP_H[i], 0);
    }
}

void OutdoorPvPAzshara::FillInitialWorldStates(WorldPacket &data)
{
    data << uint32(Azshara_UI_TOWER_DISPLAY_A) << uint32(1);
    data << uint32(Azshara_UI_TOWER_DISPLAY_H) << uint32(1);
    data << uint32(Azshara_UI_TOWER_COUNT_A) << uint32(m_AllianceTowersControlled);
    data << uint32(Azshara_UI_TOWER_COUNT_H) << uint32(m_HordeTowersControlled);
    data << uint32(Azshara_UI_TOWER_SLIDER_DISPLAY) << uint32(0);
    data << uint32(Azshara_UI_TOWER_SLIDER_POS) << uint32(50);
    data << uint32(Azshara_UI_TOWER_SLIDER_N) << uint32(100);
    for (OPvPCapturePointMap::iterator itr = m_capturePoints.begin(); itr != m_capturePoints.end(); ++itr)
    {
        itr->second->FillInitialWorldStates(data);
    }
}

void OPvPCapturePointAzshara::ChangeState()
{
    uint32 field = 0;
    switch(m_OldState)
    {
    case OBJECTIVESTATE_NEUTRAL:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE:
        field = Azshara_MAP_A[m_TowerType];
        if (((OutdoorPvPAzshara*)m_PvP)->m_AllianceTowersControlled)
            ((OutdoorPvPAzshara*)m_PvP)->m_AllianceTowersControlled--;
        sWorld->SendZoneText(OutdoorPvPAzsharaBuffZones[0], sObjectMgr->GetTrinityStringForDBCLocale(Azshara_LANG_LOSE_A[m_TowerType]));
        break;
    case OBJECTIVESTATE_HORDE:
        field = Azshara_MAP_H[m_TowerType];
        if (((OutdoorPvPAzshara*)m_PvP)->m_HordeTowersControlled)
            ((OutdoorPvPAzshara*)m_PvP)->m_HordeTowersControlled--;
        sWorld->SendZoneText(OutdoorPvPAzsharaBuffZones[0], sObjectMgr->GetTrinityStringForDBCLocale(Azshara_LANG_LOSE_H[m_TowerType]));
        break;
    case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
        field = Azshara_MAP_A[m_TowerType];
        break;
    case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
        field = Azshara_MAP_H[m_TowerType];
        break;
    }

    // send world state update
    if (field)
    {
        m_PvP->SendUpdateWorldState(field, 0);
        field = 0;
    }
    uint32 artkit = 21;
    uint32 artkit2 = Azshara_TowerArtKit_N[m_TowerType];
    switch(m_State)
    {
    case OBJECTIVESTATE_NEUTRAL:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE:
        field = Azshara_MAP_A[m_TowerType];
        artkit = 2;
        artkit2 = Azshara_TowerArtKit_A[m_TowerType];
        if (((OutdoorPvPAzshara*)m_PvP)->m_AllianceTowersControlled<3)
            ((OutdoorPvPAzshara*)m_PvP)->m_AllianceTowersControlled++;
        sWorld->SendZoneText(OutdoorPvPAzsharaBuffZones[0], sObjectMgr->GetTrinityStringForDBCLocale(Azshara_LANG_CAPTURE_A[m_TowerType]));
        break;
    case OBJECTIVESTATE_HORDE:
        field = Azshara_MAP_H[m_TowerType];
        artkit = 1;
        artkit2 = Azshara_TowerArtKit_H[m_TowerType];
        if (((OutdoorPvPAzshara*)m_PvP)->m_HordeTowersControlled<3)
            ((OutdoorPvPAzshara*)m_PvP)->m_HordeTowersControlled++;
        sWorld->SendZoneText(OutdoorPvPAzsharaBuffZones[0], sObjectMgr->GetTrinityStringForDBCLocale(Azshara_LANG_CAPTURE_H[m_TowerType]));
        break;
    case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        field = Azshara_MAP_N[m_TowerType];
        break;
    case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
        field = Azshara_MAP_A[m_TowerType];
        artkit = 2;
        artkit2 = Azshara_TowerArtKit_A[m_TowerType];
        break;
    case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
        field = Azshara_MAP_H[m_TowerType];
        artkit = 1;
        artkit2 = Azshara_TowerArtKit_H[m_TowerType];
        break;
    }

    GameObject* flag = HashMapHolder<GameObject>::Find(m_capturePointGUID);
    GameObject* flag2 = HashMapHolder<GameObject>::Find(m_Objects[m_TowerType]);
    if (flag)
    {
        flag->SetGoArtKit(artkit);
    }
    if (flag2)
    {
        flag2->SetGoArtKit(artkit2);
    }

    // send world state update
    if (field)
        m_PvP->SendUpdateWorldState(field, 1);

    // complete quest objective
    if (m_State == OBJECTIVESTATE_ALLIANCE || m_State == OBJECTIVESTATE_HORDE)
        SendObjectiveComplete(Azshara_CREDITMARKER[m_TowerType], 0);
}

void OPvPCapturePointAzshara::SendChangePhase()
{
    SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_N, m_neutralValuePct);
    // send these updates to only the ones in this objective
    uint32 phase = (uint32)ceil((m_value + m_maxValue) / (2 * m_maxValue) * 100.0f);
    SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_POS, phase);
    // send this too, sometimes the slider disappears, dunno why :(
    SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_DISPLAY, 1);
}

void OPvPCapturePointAzshara::FillInitialWorldStates(WorldPacket &data)
{
    switch(m_State)
    {
        case OBJECTIVESTATE_ALLIANCE:
        case OBJECTIVESTATE_ALLIANCE_HORDE_CHALLENGE:
            data << uint32(Azshara_MAP_N[m_TowerType]) << uint32(0);
            data << uint32(Azshara_MAP_A[m_TowerType]) << uint32(1);
            data << uint32(Azshara_MAP_H[m_TowerType]) << uint32(0);
            break;
        case OBJECTIVESTATE_HORDE:
        case OBJECTIVESTATE_HORDE_ALLIANCE_CHALLENGE:
            data << uint32(Azshara_MAP_N[m_TowerType]) << uint32(0);
            data << uint32(Azshara_MAP_A[m_TowerType]) << uint32(0);
            data << uint32(Azshara_MAP_H[m_TowerType]) << uint32(1);
            break;
        case OBJECTIVESTATE_NEUTRAL:
        case OBJECTIVESTATE_NEUTRAL_ALLIANCE_CHALLENGE:
        case OBJECTIVESTATE_NEUTRAL_HORDE_CHALLENGE:
        default:
            data << uint32(Azshara_MAP_N[m_TowerType]) << uint32(1);
            data << uint32(Azshara_MAP_A[m_TowerType]) << uint32(0);
            data << uint32(Azshara_MAP_H[m_TowerType]) << uint32(0);
            break;
    }
}

bool OPvPCapturePointAzshara::HandlePlayerEnter(Player* player)
{
    if (OPvPCapturePoint::HandlePlayerEnter(player))
    {
        player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_DISPLAY, 1);
        uint32 phase = (uint32)ceil((m_value + m_maxValue) / (2 * m_maxValue) * 100.0f);
        player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_POS, phase);
        player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_N, m_neutralValuePct);
        return true;
    }
    return false;
}

WorldSafeLocsEntry const *OPvPCapturePointAzshara::GetClosestGraveYard(Player* player)
{
    uint32 g_id = 0;

    switch (player->GetTeam())
    {
		case ALLIANCE: g_id = Azshara_Alliance_Start; break;
        case HORDE:    g_id = Azshara_Horde_Start;    break;
        default:       return NULL;
    }

    float distance, nearestDistance;

    WorldSafeLocsEntry const* entry = NULL;
    WorldSafeLocsEntry const* nearestEntry = NULL;
    entry = sWorldSafeLocsStore.LookupEntry(g_id);
    nearestEntry = entry;

    if (!entry)
    {
        sLog->outError("OutdoorPvP Azshara: Not found the main team graveyard. Graveyard system isn't working!");
        return NULL;
    }

    float plr_x = player->GetPositionX();
    float plr_y = player->GetPositionY();
    float plr_z = player->GetPositionZ();

    distance = (entry->x - plr_x)*(entry->x - plr_x) + (entry->y - plr_y)*(entry->y - plr_y) + (entry->z - plr_z)*(entry->z - plr_z);
    nearestDistance = distance;

    for (uint8 i = 0; i < 3; ++i)
    {
        if (m_PointOwnedByTeam[i] == player->GetTeam() && m_PointState[i] == 3)
        {
            entry = sWorldSafeLocsStore.LookupEntry(m_CapturingPointTypes[i].GraveYardId);
            if (!entry)
                sLog->outError("OutdoorPvP Azshara: Not found graveyard: %u", m_CapturingPointTypes[i].GraveYardId);
            else
            {
                distance = (entry->x - plr_x)*(entry->x - plr_x) + (entry->y - plr_y)*(entry->y - plr_y) + (entry->z - plr_z)*(entry->z - plr_z);
                if (distance < nearestDistance)
                {
                    nearestDistance = distance;
                    nearestEntry = entry;
                }
            }
        }
    }

    return nearestEntry;
}

void OPvPCapturePointAzshara::HandlePlayerLeave(Player* player)
{
    player->SendUpdateWorldState(Azshara_UI_TOWER_SLIDER_DISPLAY, 0);
    OPvPCapturePoint::HandlePlayerLeave(player);
}

void OutdoorPvPAzshara::HandleKillImpl(Player* player, Unit* killed)
{
    if (killed->GetTypeId() != TYPEID_PLAYER)
        return;

    if (player->GetTeam() == ALLIANCE && killed->ToPlayer()->GetTeam() != ALLIANCE)
        player->CastSpell(player, AlliancePlayerKillReward, true);
    else if (player->GetTeam() == HORDE && killed->ToPlayer()->GetTeam() != HORDE)
        player->CastSpell(player, HordePlayerKillReward, true);
}

class OutdoorPvP_Azshara : public OutdoorPvPScript
{
    public:

        OutdoorPvP_Azshara()
            : OutdoorPvPScript("outdoorpvp_azshaar")
        {
        }

        OutdoorPvP* GetOutdoorPvP() const
        {
            return new OutdoorPvPAzshara();
        }
};

void AddSC_OutdoorPvP_Azshara()
{
    new OutdoorPvP_Azshara();
}