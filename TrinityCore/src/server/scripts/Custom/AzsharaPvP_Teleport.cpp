#include "ScriptedGossip.h"
#include "ScriptMgr.h"
#include "InstanceScript.h"
#include "AzsharaPvP.h"
#include "Spell.h"
#include "GameEventMgr.h"

enum Teleports
{
	Alliance_Shop  = , 
	Horde_Shop     = ,
	Alliance_Point = ,
	Horde_Point    = ,
	Neutral_Point  =
};

enum Events
{
	Alliance_Gate_Alliance  = ,
	Alliance_Gate_Horde     = ,
	Alliance_Point_Alliance = ,
	Alliance_Point_Horde    = ,
	Neutral_Point_Alliance  = ,
	Neutral_Point_Horde     = ,
	Horde_Point_Alliance    = ,
	Horde_Point_Horde       = ,
	Horde_Gate_Alliance     = ,
	Horde_Gate_Horde        = 
};

#define GOSSIP_SENDER_AZSHARA_PORT 37

class AzsharaPvP_Teleport : public GameObjectScript
{
    public:
        AzsharaPvP_Teleport() : GameObjectScript("AzsharaPvP_Teleport") {}

        bool OnGossipHello(Player* player, GameObject* go)
        {
			if(player->GetTeam() == ALLIANCE)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Shop.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Alliance_Gate_Alliance)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Alliance Gate.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Alliance_Point_Alliance)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Alliance Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Neutral_Point_Alliance)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Graveyard Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Horde_Point_Alliance)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Horde Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Horde_Gate_Alliance)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Horde Gate.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
			}
			else if(player->GetTeam() == HORDE)
			{
				player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Shop.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Alliance_Gate_Horde)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Alliance Gate.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Alliance_Point_Horde)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Alliance Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Neutral_Point_Horde)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Graveyard Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Horde_Point_Horde)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Horde Point.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
				if(sGameEventMgr->IsActiveEvent(Horde_Gate_Horde)
					player->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, "Teleport to Horde Gate.", GOSSIP_SENDER_AZSHARA_PORT, LIGHT_S_HAMMER_TELEPORT);
			}

            player->SEND_GOSSIP_MENU(player->GetGossipTextId(go->GetGOInfo()->GetGossipMenuId()), go->GetGUID());
            return true;
        }

        bool OnGossipSelect(Player* player, GameObject* /*go*/, uint32 sender, uint32 action)
        {
            player->PlayerTalkClass->ClearMenus();
            player->CLOSE_GOSSIP_MENU();
            SpellEntry const* spell = sSpellStore.LookupEntry(action);
            if (!spell)
                return false;

            if (player->isInCombat())
            {
                Spell::SendCastResult(player, spell, 0, SPELL_FAILED_AFFECTING_COMBAT);
                return true;
            }

            if (sender == GOSSIP_SENDER_AZSHARA_PORT)
                player->CastSpell(player, spell, true);

            return true;
        }
};

class at_frozen_throne_teleport : public AreaTriggerScript
{
    public:
        at_frozen_throne_teleport() : AreaTriggerScript("at_frozen_throne_teleport") { }

        bool OnTrigger(Player* player, AreaTriggerEntry const* /*areaTrigger*/)
        {
            if (InstanceScript* instance = player->GetInstanceScript())
                if (instance->GetBossState(DATA_PROFESSOR_PUTRICIDE) == DONE && instance->GetBossState(DATA_BLOOD_QUEEN_LANA_THEL) == DONE && instance->GetBossState(DATA_SINDRAGOSA) == DONE)
                    player->CastSpell(player, FROZEN_THRONE_TELEPORT, true);

            return true;
        }
};

void AddSC_icecrown_citadel_teleport()
{
    new icecrown_citadel_teleport();
    new at_frozen_throne_teleport();
}
