/*
 * Copyright (C) 2011 Apokalypsa <http://www.apokalypsa.sk/>
 *
 * World of Warcraft: Wotlk 
 * You can edit but you can not delete this copyright.
 * TrinityCore2 
 * Pet Master
 *
 * By Smrtokvitek - Head Admin of Apokalypsa Server 
 * Special Thanks to Hkarta - "void CreatePet"
 */

/* ScriptData
SDName: npc_petmaster
SD%Complete: 100%
SDComment: Pet Master
SDCategory: Custom NPC Script
EndScriptData */

//Includes
#include "ScriptPCH.h"
#include "WorldPacket.h"
#include "ScriptMgr.h"
#include <cstring>
#include "Pet.h"
#include "Formulas.h"

//Defines
#define TEXT_BAT    "A bat is a family of cunning pets in World of Warcraft. They like to eat meat, fungus and fruit."
#define TEXT_BEAR   "A bear is a family of tenacity pets in World of Warcraft. They like to eat meat, fish, cheese, bread, fungus and fruit."
#define TEXT_BIRD_OF_PREY   "A bird of prey is a family of cunning pets in World of Warcraft. They like to eat meat and fish."
#define TEXT_BOAR   "A boar is a family of tenacity pets in World of Warcraft. They like to eat meat, fish, cheese, bread, fungus and fruit."
#define TEXT_CARRION_BIRD   "A carrion bird is a family of ferocity pets in World of Warcraft. They like to eat meat and fish."
#define TEXT_CAT   "A cat is a family of ferocity pets in World of Warcraft. They like to eat meat and fish."
#define TEXT_CHIMAERA   "A chimaera is a family of exotic cunning pets in World of Warcraft. They like to eat meat."
#define TEXT_CORE_HOUND   "A core hound is a family of exotic ferocity pets in World of Warcraft. They like to eat meat."
#define TEXT_CRAB   "A crab is a family of tenacity pets in World of Warcraft. They like to eat fish, bread, fungus and fruit."
#define TEXT_CROCOLISK   "A crocolisk is a family of tenacity pets in World of Warcraft. They like to eat meat and fish."
#define TEXT_DEVILSAUR   "A devilsaur is a family of exotic ferocity pets in World of Warcraft. They like to eat meat."
#define TEXT_DRAGONHAWK   "A dragonhawk is a family of cunning pets in World of Warcraft. They like to eat meat, fish and fruit."
#define TEXT_GORILLA   "A gorilla is a family of tenacity pets in World of Warcraft. They like to eat bread, fungus and fruit."
#define TEXT_HYENA   "A hyena is a family of ferocity pets in World of Warcraft. They like to eat meat."
#define TEXT_MOTH   "A moth is a family of ferocity pets in World of Warcraft. They like to eat cheese, bread, fungus and fruit."
#define TEXT_NETHER_RAY   "A nether ray is a family of cunning pets in World of Warcraft. They like to eat meat and fungus."
#define TEXT_RAPTOR   "A raptor is a family of ferocity pets in World of Warcraft. They like to eat meat."
#define TEXT_RAVAGER   "A ravager is a family of cunning pets in World of Warcraft. They like to eat meat."
#define TEXT_RHINO   "A rhino is a family of exotic tenacity pets in World of Warcraft. They like to eat cheese, bread, fungus and fruit."
#define TEXT_SCORPID   "A scorpid is a family of tenacity pets in World of Warcraft. They like to eat meat."
#define TEXT_SERPENT   "A serpent is a family of cunning pets in World of Warcraft. They like to eat meat."
#define TEXT_SILITHID   "A silithid is a family of exotic cunning pets in World of Warcraft. They like to eat meat and fungus."
#define TEXT_SPIDER   "A spider is a family of cunning pets in World of Warcraft. They like to eat meat."
#define TEXT_SPIRIT_BEAST   "A spirit beast is a family of exotic ferocity pets in World of Warcraft. They like to eat meat and fish."
#define TEXT_SPOREBAT   "A sporebat is a family of cunning pets in World of Warcraft. They like to eat cheese, bread, fungus and fruit."
#define TEXT_TALLSTRIDER   "A tallstrider is a family of ferocity pets in World of Warcraft. They like to eat cheese, bread, fungus and fruit."
#define TEXT_TURTLE   "A turtle is a family of tenacity pets in World of Warcraft. They like to eat fish, bread, fungus and fruit."
#define TEXT_WARP_STALKER   "A warp stalker is a family of tenacity pets in World of Warcraft. They like to eat fish and fruit."
#define TEXT_WASP   "A wasp is a family of ferocity pets in World of Warcraft. They like to eat cheese, bread, fungus and fruit."
#define TEXT_WIND_SERPENT   "A wind serpent is a family of cunning pets in World of Warcraft. They like to eat fish, cheese and bread."
#define TEXT_WOLF   "A wolf is a family of ferocity pets in World of Warcraft. They like to eat meat."
#define TEXT_WORM   "A worm is a family of exotic tenacity pets in World of Warcraft. They like to eat cheese, bread and fungus."

void CreatePet( Player *player, Creature * m_creature, uint32 entry )
{
    if(player->getClass() != CLASS_HUNTER)
    {
        m_creature->MonsterWhisper("You are not hunter!", player->GetGUID());
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    if(player->GetPet())
    {
        m_creature->MonsterWhisper("First you must drop your pet!", player->GetGUID());
        player->CLOSE_GOSSIP_MENU();
        return;
    }

    Creature *creatureTarget = m_creature->SummonCreature(entry, player->GetPositionX(), player->GetPositionY()+2, player->GetPositionZ(), player->GetOrientation(), TEMPSUMMON_CORPSE_TIMED_DESPAWN, 500);

    if(!creatureTarget)
        return;
    
    Pet* pet = player->CreateTamedPetFrom(creatureTarget, 0);

    if(!pet)
        return;

    creatureTarget->setDeathState(JUST_DIED);
    creatureTarget->RemoveCorpse();
    creatureTarget->SetHealth(0);
    pet->SetPower(POWER_HAPPINESS, 1048000);
    pet->GetMap()->Add((Creature*)pet);
    pet->SetUInt32Value(UNIT_FIELD_LEVEL, uint32(player->getLevel()));

    if(!pet->InitStatsForLevel(pet->getLevel()))
        sLog->outError("Pet Create fail: no init stats for entry %u", entry);

    pet->UpdateAllStats();
    player->SetMinion(pet, true);
    pet->SavePetToDB(PET_SAVE_AS_CURRENT);
    pet->InitTalentForLevel();
    player->PetSpellInitialize();
    player->CLOSE_GOSSIP_MENU();
    m_creature->MonsterWhisper("Pet added. You might want to feed it and name it somehow.", player->GetGUID());
}

class npc_petmaster : public CreatureScript
{
    public:
        npc_petmaster() : CreatureScript("npc_petmaster") { }

	 	bool OnGossipHello(Player *player, Creature *_creature)
        	{
                     if(player->getClass() != CLASS_HUNTER)
                     {
                            _creature->MonsterWhisper("You are not hunter!", player->GetGUID());
                   		return true;
              	}
			if (player->isInCombat())
            		{
                		player->CLOSE_GOSSIP_MENU();
                		_creature->MonsterWhisper("Combat!", player->GetGUID());
              		return true;
            		}
	       	else
			{
	  			player->ADD_GOSSIP_ITEM( 6, "Classic"                     , GOSSIP_SENDER_MAIN, 1);
	  			player->ADD_GOSSIP_ITEM( 6, "The Burning Crusader"        , GOSSIP_SENDER_MAIN, 3);
	  			player->ADD_GOSSIP_ITEM( 6, "Wrath of the Lich King"      , GOSSIP_SENDER_MAIN, 4);
	  	     	}

	  	player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());         
	  	return true;
        	}

	 	bool OnGossipSelect(Player *player, Creature *_creature, uint32 sender, uint32 uiAction)
	 	{
		    if (player->isInCombat())
		    {
                  		player->CLOSE_GOSSIP_MENU();
                  		_creature->MonsterWhisper("Combat !", player->GetGUID());
                		return true;
	           }
            	    if (sender == GOSSIP_SENDER_MAIN)
            	    {
			 player->PlayerTalkClass->ClearMenus();
                      switch(uiAction)
                  	 {
        		 case 1: //Classic I
	  			player->ADD_GOSSIP_ITEM( 1, "Bat"                            , GOSSIP_SENDER_MAIN, 100);
	  			player->ADD_GOSSIP_ITEM( 1, "Bear"                           , GOSSIP_SENDER_MAIN, 101);
	  			player->ADD_GOSSIP_ITEM( 1, "Bird of Prey"                   , GOSSIP_SENDER_MAIN, 102);
	  			player->ADD_GOSSIP_ITEM( 1, "Boar"                           , GOSSIP_SENDER_MAIN, 105);
	  			player->ADD_GOSSIP_ITEM( 1, "Carrion Bird"                   , GOSSIP_SENDER_MAIN, 106);
	  			player->ADD_GOSSIP_ITEM( 1, "Cat"                            , GOSSIP_SENDER_MAIN, 107);
	  			player->ADD_GOSSIP_ITEM( 1, "Crab"                           , GOSSIP_SENDER_MAIN, 110);
	  			player->ADD_GOSSIP_ITEM( 1, "Crocolisk"                      , GOSSIP_SENDER_MAIN, 111);
	  			player->ADD_GOSSIP_ITEM( 1, "Gorilla"                        , GOSSIP_SENDER_MAIN, 112);
				player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"      	     	     , GOSSIP_SENDER_MAIN, 2);
				player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 	break;

        		 	case 100: //Bat
	  				player->ADD_GOSSIP_ITEM( 8, "Normal"                  , GOSSIP_SENDER_MAIN, 1000);
	  				player->ADD_GOSSIP_ITEM( 8, "Brown"                   , GOSSIP_SENDER_MAIN, 1001);
	  				player->ADD_GOSSIP_ITEM( 8, "Violet"                  , GOSSIP_SENDER_MAIN, 1002);
	  				player->ADD_GOSSIP_ITEM( 8, "White"                   , GOSSIP_SENDER_MAIN, 1003);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1000: //Normal
            					CreatePet(player, _creature, 4538);
                  				_creature->MonsterWhisper(TEXT_BAT, player->GetGUID());
           					break;

        				case 1001: //Brown
            					CreatePet(player, _creature, 1512);
                  				_creature->MonsterWhisper(TEXT_BAT, player->GetGUID());
           					break;

        				case 1002: //Violet
            					CreatePet(player, _creature, 48456);
                  				_creature->MonsterWhisper(TEXT_BAT, player->GetGUID());
           					break;

        				case 1003: //White
            					CreatePet(player, _creature, 28233);
                  				_creature->MonsterWhisper(TEXT_BAT, player->GetGUID());
           					break;

        		 	case 101: //Bear
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                          , GOSSIP_SENDER_MAIN, 1010);
	  				player->ADD_GOSSIP_ITEM( 1, "Black Diseased"                 , GOSSIP_SENDER_MAIN, 1011);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                           , GOSSIP_SENDER_MAIN, 1012);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue Diseased"                  , GOSSIP_SENDER_MAIN, 1013);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"                          , GOSSIP_SENDER_MAIN, 1014);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown Diseased"                 , GOSSIP_SENDER_MAIN, 1015);
	  				player->ADD_GOSSIP_ITEM( 1, "Dark Brown"                     , GOSSIP_SENDER_MAIN, 1016);
	  				player->ADD_GOSSIP_ITEM( 1, "Dark Brown Diseased"            , GOSSIP_SENDER_MAIN, 1017);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                          , GOSSIP_SENDER_MAIN, 1018);
	  				player->ADD_GOSSIP_ITEM( 1, "White Diseased"                 , GOSSIP_SENDER_MAIN, 1019);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1010: //Black
            					CreatePet(player, _creature, 1186);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1011: //Black Diseased
            					CreatePet(player, _creature, 1815);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1012: //Blue
            					CreatePet(player, _creature, 1196);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1013: //Blue Diseased
            					CreatePet(player, _creature, 47204);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1014: //Brown
            					CreatePet(player, _creature, 17345);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1015: //Brown Diseased
            					CreatePet(player, _creature, 14280);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1016: //Dark Brown
            					CreatePet(player, _creature, 3809);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1017: //Dark Brown Diseased
            					CreatePet(player, _creature, 8956);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1018: //White
            					CreatePet(player, _creature, 26482);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        				case 1019: //White Diseased
            					CreatePet(player, _creature, 26643);
                  				_creature->MonsterWhisper(TEXT_BEAR, player->GetGUID());
           					break;

        		 	case 102: //Bird of Prey I
	  				player->ADD_GOSSIP_ITEM( 1, "Brown Eagle"                    , GOSSIP_SENDER_MAIN, 1020);
	  				player->ADD_GOSSIP_ITEM( 1, "Carrion Bird Brown"             , GOSSIP_SENDER_MAIN, 1021);
	  				player->ADD_GOSSIP_ITEM( 1, "Eagle"                          , GOSSIP_SENDER_MAIN, 1022);
	  				player->ADD_GOSSIP_ITEM( 1, "Light Brown Eagle"              , GOSSIP_SENDER_MAIN, 1023);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Arrokoa Green"              , GOSSIP_SENDER_MAIN, 1024);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Arrokoa Purple"             , GOSSIP_SENDER_MAIN, 1025);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Arrokoa Red"                , GOSSIP_SENDER_MAIN, 1026);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Black"                      , GOSSIP_SENDER_MAIN, 1027);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Blue"                       , GOSSIP_SENDER_MAIN, 1028);
	  				player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"                   , GOSSIP_SENDER_MAIN, 103);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1020: //Brown Eagle
            					CreatePet(player, _creature, 23693);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1021: //Carrion Bird Brown
            					CreatePet(player, _creature, 51760);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1022: //Eagle
            					CreatePet(player, _creature, 42657);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1023: //Light Brown Eagle
            					CreatePet(player, _creature, 44628);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1024: //Owl Arrokoa Green
            					CreatePet(player, _creature, 21904);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1025: //Owl Arrokoa Purple
            					CreatePet(player, _creature, 21904);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1026: //Owl Arrokoa Red
            					CreatePet(player, _creature, 44020);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1027: //Owl Black
            					CreatePet(player, _creature, 44020);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1028: //Owl Blue
            					CreatePet(player, _creature, 50044);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        		 	case 103: //Bird of Prey II
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Brown"                      , GOSSIP_SENDER_MAIN, 1030);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Gray"                       , GOSSIP_SENDER_MAIN, 1031);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Jade"                       , GOSSIP_SENDER_MAIN, 1032);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl Spirit"                     , GOSSIP_SENDER_MAIN, 1033);
	  				player->ADD_GOSSIP_ITEM( 1, "Owl White"                      , GOSSIP_SENDER_MAIN, 1034);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Blue"                    , GOSSIP_SENDER_MAIN, 1035);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Crimson"                 , GOSSIP_SENDER_MAIN, 1036);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Gray"                    , GOSSIP_SENDER_MAIN, 1037);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Green"                   , GOSSIP_SENDER_MAIN, 1038);
	  				player->ADD_GOSSIP_ITEM( 7, "[Page III] ->"                  , GOSSIP_SENDER_MAIN, 104);
	  				player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"                    , GOSSIP_SENDER_MAIN, 102);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1030: //Owl Brown
            					CreatePet(player, _creature, 17034);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1031: //Owl Gray
            					CreatePet(player, _creature, 7097);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1032: //Owl Jade
            					CreatePet(player, _creature, 34982);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1033: //Owl Spirit
            					CreatePet(player, _creature, 14343);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1034: //Owl White
            					CreatePet(player, _creature, 14283);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1035: //Parrot Blue
            					CreatePet(player, _creature, 32481);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1036: //Parrot Crimson
            					CreatePet(player, _creature, 48137);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1037: //Parrot Gray
            					CreatePet(player, _creature, 48451);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1038: //Parrot Green
            					CreatePet(player, _creature, 48138);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        		 	case 104: //Bird of Prey III
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Lavender"                , GOSSIP_SENDER_MAIN, 1040);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Orange"                  , GOSSIP_SENDER_MAIN, 1041);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot Red"                     , GOSSIP_SENDER_MAIN, 1042);
	  				player->ADD_GOSSIP_ITEM( 1, "Parrot White"                   , GOSSIP_SENDER_MAIN, 1043);
	  				player->ADD_GOSSIP_ITEM( 1, "Snowy Eagle"                    , GOSSIP_SENDER_MAIN, 1044);
	  				player->ADD_GOSSIP_ITEM( 1, "Storm Crow Druid"               , GOSSIP_SENDER_MAIN, 1045);
	  				player->ADD_GOSSIP_ITEM( 1, "Storm Crow Druid Brown"         , GOSSIP_SENDER_MAIN, 1046);
	  				player->ADD_GOSSIP_ITEM( 1, "Storm Crow"                     , GOSSIP_SENDER_MAIN, 1047);
	  				player->ADD_GOSSIP_ITEM( 1, "Undead Eagle"                   , GOSSIP_SENDER_MAIN, 1048);
	  				player->ADD_GOSSIP_ITEM( 7, "<- [Page II]"                   , GOSSIP_SENDER_MAIN, 103);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1040: //Parrot Lavender
            					CreatePet(player, _creature, 48449);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1041: //Parrot Orange
            					CreatePet(player, _creature, 48450);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1042: //Parrot Red
            					CreatePet(player, _creature, 48447);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1043: //Parrot White
            					CreatePet(player, _creature, 48448);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1044: //Snowy Eagle
            					CreatePet(player, _creature, 29044);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1045: //Storm Crow Druid
            					CreatePet(player, _creature, 24172);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1046: //Storm Crow Druid Brown
            					CreatePet(player, _creature, 24747);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1047: //Storm Crow
            					CreatePet(player, _creature, 21042);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        				case 1048: //Undead Eagle
            					CreatePet(player, _creature, 31263);
                  				_creature->MonsterWhisper(TEXT_BIRD_OF_PREY, player->GetGUID());
           					break;

        		 	case 105: //Boar
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                           , GOSSIP_SENDER_MAIN, 1050);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue Armored"                   , GOSSIP_SENDER_MAIN, 1051);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"                          , GOSSIP_SENDER_MAIN, 1052);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown Armored"                  , GOSSIP_SENDER_MAIN, 1053);
	  				player->ADD_GOSSIP_ITEM( 1, "Crimson"                        , GOSSIP_SENDER_MAIN, 1054);
	  				player->ADD_GOSSIP_ITEM( 1, "Ivory"                          , GOSSIP_SENDER_MAIN, 1055);
	  				player->ADD_GOSSIP_ITEM( 1, "Ivory Armored"                  , GOSSIP_SENDER_MAIN, 1056);
	  				player->ADD_GOSSIP_ITEM( 1, "Undead"                         , GOSSIP_SENDER_MAIN, 1057);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                         , GOSSIP_SENDER_MAIN, 1058);
	  				player->ADD_GOSSIP_ITEM( 1, "Fel"                            , GOSSIP_SENDER_MAIN, 1059);
	  				player->ADD_GOSSIP_ITEM( 1, "Fel Black"                      , GOSSIP_SENDER_MAIN, 1060);
	  				player->ADD_GOSSIP_ITEM( 1, "Fel Blue"                       , GOSSIP_SENDER_MAIN, 1061);
	  				player->ADD_GOSSIP_ITEM( 1, "Fel Gray"                       , GOSSIP_SENDER_MAIN, 1062);
	  				player->ADD_GOSSIP_ITEM( 1, "Fel Yellow"                     , GOSSIP_SENDER_MAIN, 1063);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1050: //Blue
            					CreatePet(player, _creature, 44089);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1051: //Blue Armored
            					CreatePet(player, _creature, 4512);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1052: //Brown 
            					CreatePet(player, _creature, 345);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1053: //Brown Armored
            					CreatePet(player, _creature, 4511);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1054: //Crimson
            					CreatePet(player, _creature, 157);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1055: //Ivory
            					CreatePet(player, _creature, 3100);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1056: //Ivory Armored
            					CreatePet(player, _creature, 4514);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1057: //Undead
            					CreatePet(player, _creature, 16117);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1058: //Yellow
            					CreatePet(player, _creature, 2966);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1059: //Fel
            					CreatePet(player, _creature, 5993);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1060: //Fel Black
            					CreatePet(player, _creature, 21878);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1061: //Fel Blue
            					CreatePet(player, _creature, 22180);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1062: //Fel Gray
            					CreatePet(player, _creature, 16863);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        				case 1063: //Fel Yellow
            					CreatePet(player, _creature, 21195);
                  				_creature->MonsterWhisper(TEXT_BOAR, player->GetGUID());
           					break;

        		 	case 106: //Carrion Bird
	  				player->ADD_GOSSIP_ITEM( 1, "Arctic Blue"                    , GOSSIP_SENDER_MAIN, 1064);
	  				player->ADD_GOSSIP_ITEM( 1, "Arctic Gold"                    , GOSSIP_SENDER_MAIN, 1065);
	  				player->ADD_GOSSIP_ITEM( 1, "Arctic White"                   , GOSSIP_SENDER_MAIN, 1066);
	  				player->ADD_GOSSIP_ITEM( 1, "Bird"                           , GOSSIP_SENDER_MAIN, 1067);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue Bird"                      , GOSSIP_SENDER_MAIN, 1068);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown Bird"                     , GOSSIP_SENDER_MAIN, 1069);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Bird"                   , GOSSIP_SENDER_MAIN, 1070);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Blue Bird"              , GOSSIP_SENDER_MAIN, 1071);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Green Bird"             , GOSSIP_SENDER_MAIN, 1072);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland White Bird"             , GOSSIP_SENDER_MAIN, 1073);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Yellow Bird"            , GOSSIP_SENDER_MAIN, 1074);
	  				player->ADD_GOSSIP_ITEM( 1, "Red Bird"                       , GOSSIP_SENDER_MAIN, 1075);
	  				player->ADD_GOSSIP_ITEM( 1, "White Bird"                     , GOSSIP_SENDER_MAIN, 1076);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1064: //Arctic Blue
            					CreatePet(player, _creature, 26483);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1065: //Arctic Gold
            					CreatePet(player, _creature, 428);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1066: //Arctic White
            					CreatePet(player, _creature, 26174);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1067: //Bird
            					CreatePet(player, _creature, 34640);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1068: //Blue Bird
            					CreatePet(player, _creature, 18470);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1069: //Brown Bird
            					CreatePet(player, _creature, 4695);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1070: //Outland Bird
            					CreatePet(player, _creature, 16972);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1071: //Outland Blue Bird
            					CreatePet(player, _creature, 18707);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1072: //Outland Green Bird
            					CreatePet(player, _creature, 5983);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1073: //Outland White Bird
            					CreatePet(player, _creature, 21515);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1074: //Outland Yellow Bird
            					CreatePet(player, _creature, 8299);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1075: //Red Bird
            					CreatePet(player, _creature, 5429);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        				case 1076: //White Bird
            					CreatePet(player, _creature, 2931);
                  				_creature->MonsterWhisper(TEXT_CARRION_BIRD, player->GetGUID());
           					break;

        		 	case 107: //Cat I
	  				player->ADD_GOSSIP_ITEM( 1, "Lioness Gold"                     , GOSSIP_SENDER_MAIN, 1077);
	  				player->ADD_GOSSIP_ITEM( 1, "Lion Black"             		, GOSSIP_SENDER_MAIN, 1078);
	  				player->ADD_GOSSIP_ITEM( 1, "Lion Gold"                        , GOSSIP_SENDER_MAIN, 1079);
	  				player->ADD_GOSSIP_ITEM( 1, "Lion White"              		, GOSSIP_SENDER_MAIN, 1080);
	  				player->ADD_GOSSIP_ITEM( 1, "Lynx Red"              		, GOSSIP_SENDER_MAIN, 1081);
	  				player->ADD_GOSSIP_ITEM( 1, "Lynx Yellow"             		, GOSSIP_SENDER_MAIN, 1082);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Aqua"               		, GOSSIP_SENDER_MAIN, 1083);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Blacknosaddle"              , GOSSIP_SENDER_MAIN, 1084);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Darknosaddle"               , GOSSIP_SENDER_MAIN, 1085);
	  				player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"                     , GOSSIP_SENDER_MAIN, 108);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1077: //Lioness Gold
            					CreatePet(player, _creature, 14222);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1078: //Lion Black
            					CreatePet(player, _creature, 5828);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1079: //Lion Gold
            					CreatePet(player, _creature, 37207);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1080: //Lion White
            					CreatePet(player, _creature, 3475);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1081: //Lynx Red
            					CreatePet(player, _creature, 15366);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1082: //Lynx Yellow
            					CreatePet(player, _creature, 44188);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1083: //Tiger Aqua
            					CreatePet(player, _creature, 10200);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1084: //Tiger Blacknosaddle
            					CreatePet(player, _creature, 47053);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1085: //Tiger Darknosaddle
            					CreatePet(player, _creature, 2175);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        		 	case 108: //Cat II
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Lavender"                   , GOSSIP_SENDER_MAIN, 1086);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Rednosaddle"                , GOSSIP_SENDER_MAIN, 1087);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Snownosaddle"               , GOSSIP_SENDER_MAIN, 1088);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Whitenosaddle"              , GOSSIP_SENDER_MAIN, 1089);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Yellownosaddle"             , GOSSIP_SENDER_MAIN, 1090);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Black"                      , GOSSIP_SENDER_MAIN, 1091);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Black Spotted"              , GOSSIP_SENDER_MAIN, 1092);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Black Striped"              , GOSSIP_SENDER_MAIN, 1093);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Brown"                      , GOSSIP_SENDER_MAIN, 1094);
	  				player->ADD_GOSSIP_ITEM( 7, "[Page III] ->"                    , GOSSIP_SENDER_MAIN, 109);
	  				player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"                      , GOSSIP_SENDER_MAIN, 107);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1086: //Tiger Lavender
            					CreatePet(player, _creature, 7434);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1087: //Tiger Rednosaddle
            					CreatePet(player, _creature, 28297);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1088: //Tiger Snownosaddle
            					CreatePet(player, _creature, 14430);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1089: //Tiger Whitenosaddle
            					CreatePet(player, _creature, 10200);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1090: //Tiger Yellownosaddle
            					CreatePet(player, _creature, 28001);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1091: //Tiger Black
            					CreatePet(player, _creature, 728);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1092: //Tiger Black Spotted
            					CreatePet(player, _creature, 28404);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1093: //Tiger Black Striped
            					CreatePet(player, _creature, 21723);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1094: //Tiger Brown
            					CreatePet(player, _creature, 2850);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        		 	case 109: //Cat III
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Dark"                	, GOSSIP_SENDER_MAIN, 1095);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Green"                      , GOSSIP_SENDER_MAIN, 1096);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Nostripe White"             , GOSSIP_SENDER_MAIN, 1097);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Red"                   	, GOSSIP_SENDER_MAIN, 1098);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Snow"                    	, GOSSIP_SENDER_MAIN, 1099);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger White"               	, GOSSIP_SENDER_MAIN, 1100);
	  				player->ADD_GOSSIP_ITEM( 1, "Tiger Yellow"         		, GOSSIP_SENDER_MAIN, 1101);
	  				player->ADD_GOSSIP_ITEM( 7, "<- [Page II]"                     , GOSSIP_SENDER_MAIN, 108);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1095: //Tiger Dark
            					CreatePet(player, _creature, 2033);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1096: //Tiger Green
            					CreatePet(player, _creature, 47687);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1097: //Tiger Nostripe White
            					CreatePet(player, _creature, 7433);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1098: //Tiger Red
            					CreatePet(player, _creature, 698);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1099: //Tiger Snow
            					CreatePet(player, _creature, 29327);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1100: //Tiger White
            					CreatePet(player, _creature, 7431);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        				case 1101: //Tiger Yellow
            					CreatePet(player, _creature, 4126);
                  				_creature->MonsterWhisper(TEXT_CAT, player->GetGUID());
           					break;

        		 	case 110: //Crab
	  				player->ADD_GOSSIP_ITEM( 1, "Bronze"                           , GOSSIP_SENDER_MAIN, 1102);
	  				player->ADD_GOSSIP_ITEM( 1, "Ivory"                            , GOSSIP_SENDER_MAIN, 1103);
	  				player->ADD_GOSSIP_ITEM( 1, "Saphire"                          , GOSSIP_SENDER_MAIN, 1104);
	  				player->ADD_GOSSIP_ITEM( 1, "Vermillian"                       , GOSSIP_SENDER_MAIN, 1105);
	  				player->ADD_GOSSIP_ITEM( 1, "Deep Sea Black"                   , GOSSIP_SENDER_MAIN, 1106);
	  				player->ADD_GOSSIP_ITEM( 1, "Deep Sea Blue"                    , GOSSIP_SENDER_MAIN, 1107);
	  				player->ADD_GOSSIP_ITEM( 1, "Deep Sea Orange"                  , GOSSIP_SENDER_MAIN, 1108);
	  				player->ADD_GOSSIP_ITEM( 1, "Deep Sea Green"                   , GOSSIP_SENDER_MAIN, 1109);
	  				player->ADD_GOSSIP_ITEM( 1, "Deep Sea Red"                     , GOSSIP_SENDER_MAIN, 1110);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1102: //Bronze
            					CreatePet(player, _creature, 17217);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1103: //Ivory
            					CreatePet(player, _creature, 4823);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1104: //Saphire
            					CreatePet(player, _creature, 3812);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1105: //Vermillian
            					CreatePet(player, _creature, 24287);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1106: //Deep Sea Black
            					CreatePet(player, _creature, 41922);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1107: //Deep Sea Blue
            					CreatePet(player, _creature, 44390);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1108: //Deep Sea Orange
            					CreatePet(player, _creature, 40238);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1109: //Deep Sea Green
            					CreatePet(player, _creature, 46369);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        				case 1110: //Deep Sea Red
            					CreatePet(player, _creature, 40798);
                  				_creature->MonsterWhisper(TEXT_CRAB, player->GetGUID());
           					break;

        		 	case 111: //Crocolisk
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                            , GOSSIP_SENDER_MAIN, 1111);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                             , GOSSIP_SENDER_MAIN, 1112);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"                            , GOSSIP_SENDER_MAIN, 1113);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"                            , GOSSIP_SENDER_MAIN, 1114);
	  				player->ADD_GOSSIP_ITEM( 1, "Pale"                             , GOSSIP_SENDER_MAIN, 1115);
	  				player->ADD_GOSSIP_ITEM( 1, "Albino"                           , GOSSIP_SENDER_MAIN, 1116);
	  				player->ADD_GOSSIP_ITEM( 1, "Marsh"                            , GOSSIP_SENDER_MAIN, 1117);
	  				player->ADD_GOSSIP_ITEM( 1, "River"                            , GOSSIP_SENDER_MAIN, 1118);
	  				player->ADD_GOSSIP_ITEM( 1, "Swamp"                            , GOSSIP_SENDER_MAIN, 1119);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1111: //Black
            					CreatePet(player, _creature, 46278);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1112: //Blue
            					CreatePet(player, _creature, 47591);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1113: //Brown
            					CreatePet(player, _creature, 43658);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1114: //Green
            					CreatePet(player, _creature, 2476);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1115: //Pale
            					CreatePet(player, _creature, 45896);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1116: //Albino
            					CreatePet(player, _creature, 20773);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1117: //Marsh
            					CreatePet(player, _creature, 39452);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1118: //River
            					CreatePet(player, _creature, 3110);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        				case 1119: //Swamp
            					CreatePet(player, _creature, 4341);
                  				_creature->MonsterWhisper(TEXT_CROCOLISK, player->GetGUID());
           					break;

        		 	case 112: //Gorilla
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                            , GOSSIP_SENDER_MAIN, 1120);
	  				player->ADD_GOSSIP_ITEM( 1, "Gray"                             , GOSSIP_SENDER_MAIN, 1121);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                              , GOSSIP_SENDER_MAIN, 1122);
	  				player->ADD_GOSSIP_ITEM( 1, "Silver"                           , GOSSIP_SENDER_MAIN, 1123);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                            , GOSSIP_SENDER_MAIN, 1124);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	       , GOSSIP_SENDER_MAIN, 1);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	       , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 1120: //Black
            					CreatePet(player, _creature, 28096);
                  				_creature->MonsterWhisper(TEXT_GORILLA, player->GetGUID());
           					break;

        				case 1121: //Gray
            					CreatePet(player, _creature, 1557);
                  				_creature->MonsterWhisper(TEXT_GORILLA, player->GetGUID());
           					break;

        				case 1122: //Red
            					CreatePet(player, _creature, 5260);
                  				_creature->MonsterWhisper(TEXT_GORILLA, player->GetGUID());
           					break;

        				case 1123: //Silver
            					CreatePet(player, _creature, 1511);
                  				_creature->MonsterWhisper(TEXT_GORILLA, player->GetGUID());
           					break;

        				case 1124: //White
            					CreatePet(player, _creature, 51661);
                  				_creature->MonsterWhisper(TEXT_GORILLA, player->GetGUID());
           					break;

        		 case 2: //Classic II
	  			player->ADD_GOSSIP_ITEM( 1, "Hyena"                          , GOSSIP_SENDER_MAIN, 200);
	  			player->ADD_GOSSIP_ITEM( 1, "Raptor"                         , GOSSIP_SENDER_MAIN, 201);
	  			player->ADD_GOSSIP_ITEM( 1, "Scorpid"                        , GOSSIP_SENDER_MAIN, 202);
	  			player->ADD_GOSSIP_ITEM( 1, "Serpent"                        , GOSSIP_SENDER_MAIN, 203);
	  			player->ADD_GOSSIP_ITEM( 1, "Spider"                         , GOSSIP_SENDER_MAIN, 204);
	  			player->ADD_GOSSIP_ITEM( 1, "Tallstrider"                    , GOSSIP_SENDER_MAIN, 206);
	  			player->ADD_GOSSIP_ITEM( 1, "Turtle"                         , GOSSIP_SENDER_MAIN, 207);
	  			player->ADD_GOSSIP_ITEM( 1, "Wind Serpent"                   , GOSSIP_SENDER_MAIN, 208);
	  			player->ADD_GOSSIP_ITEM( 1, "Wolf"                           , GOSSIP_SENDER_MAIN, 209);
				player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"      	     	     , GOSSIP_SENDER_MAIN, 1);	  							
				player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 	break;

        		 	case 200: //Hyena
	  				player->ADD_GOSSIP_ITEM( 1, "Hyena"                          , GOSSIP_SENDER_MAIN, 2000);
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                          , GOSSIP_SENDER_MAIN, 2001);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                           , GOSSIP_SENDER_MAIN, 2002);
	  				player->ADD_GOSSIP_ITEM( 1, "Orange"                         , GOSSIP_SENDER_MAIN, 2003);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                            , GOSSIP_SENDER_MAIN, 2004);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                          , GOSSIP_SENDER_MAIN, 2005);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                         , GOSSIP_SENDER_MAIN, 2006);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2000: //Hyena
            					CreatePet(player, _creature, 5426);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2001: //Black
            					CreatePet(player, _creature, 4688);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2002: //Blue
            					CreatePet(player, _creature, 4248);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2003: //Orange
            					CreatePet(player, _creature, 48209);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2004: //Red
            					CreatePet(player, _creature, 45353);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2005: //White
            					CreatePet(player, _creature, 14228);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        				case 2006: //Yellow
            					CreatePet(player, _creature, 12418);
                  				_creature->MonsterWhisper(TEXT_HYENA, player->GetGUID());
           					break;

        		 	case 201: //Raptor
	  				player->ADD_GOSSIP_ITEM( 1, "Gray"                           , GOSSIP_SENDER_MAIN, 2010);
	  				player->ADD_GOSSIP_ITEM( 1, "Mottled Blue Green"             , GOSSIP_SENDER_MAIN, 2011);
	  				player->ADD_GOSSIP_ITEM( 1, "Mottled Dark Green"             , GOSSIP_SENDER_MAIN, 2012);
	  				player->ADD_GOSSIP_ITEM( 1, "Orange"                         , GOSSIP_SENDER_MAIN, 2013);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                            , GOSSIP_SENDER_MAIN, 2014);
	  				player->ADD_GOSSIP_ITEM( 1, "Violet"                         , GOSSIP_SENDER_MAIN, 2015);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                         , GOSSIP_SENDER_MAIN, 2016);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Black"                  , GOSSIP_SENDER_MAIN, 2017);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Green"                  , GOSSIP_SENDER_MAIN, 2018);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Red"                    , GOSSIP_SENDER_MAIN, 2019);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Yellow"                 , GOSSIP_SENDER_MAIN, 2020);
	  				player->ADD_GOSSIP_ITEM( 1, "Obsidian"                       , GOSSIP_SENDER_MAIN, 2021);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2010: //Gray
            					CreatePet(player, _creature, 40412);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2011: //Mottled Blue Green
            					CreatePet(player, _creature, 3637);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2012: //Mottled Dark Green
            					CreatePet(player, _creature, 41401);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2013: //Orange
            					CreatePet(player, _creature, 14232);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2014: //Red
            					CreatePet(player, _creature, 4357);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2015: //Violet
            					CreatePet(player, _creature, 4632);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2016: //Yellow
            					CreatePet(player, _creature, 2561);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2017: //Outland Black
            					CreatePet(player, _creature, 20729);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2018: //Outland Green
            					CreatePet(player, _creature, 26641);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2019: //Outland Red
            					CreatePet(player, _creature, 37961);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2020: //Outland Yellow
            					CreatePet(player, _creature, 20728);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        				case 2021: //Obsidian
            					CreatePet(player, _creature, 23873);
                  				_creature->MonsterWhisper(TEXT_RAPTOR, player->GetGUID());
           					break;

        		 	case 202: //Scorpid
	  				player->ADD_GOSSIP_ITEM( 1, "Horde"                           , GOSSIP_SENDER_MAIN, 2022);
	  				player->ADD_GOSSIP_ITEM( 1, "Horde Black"              	      , GOSSIP_SENDER_MAIN, 2023);
	  				player->ADD_GOSSIP_ITEM( 1, "Horde Blue"             	      , GOSSIP_SENDER_MAIN, 2024);
	  				player->ADD_GOSSIP_ITEM( 1, "Horde Green"                     , GOSSIP_SENDER_MAIN, 2025);
	  				player->ADD_GOSSIP_ITEM( 1, "Horde Red"                       , GOSSIP_SENDER_MAIN, 2026);
	  				player->ADD_GOSSIP_ITEM( 1, "Beiqe"                           , GOSSIP_SENDER_MAIN, 2027);
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                           , GOSSIP_SENDER_MAIN, 2028);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                  	      , GOSSIP_SENDER_MAIN, 2029);
	  				player->ADD_GOSSIP_ITEM( 1, "Dark Iron"                       , GOSSIP_SENDER_MAIN, 2030);
	  				player->ADD_GOSSIP_ITEM( 1, "Golden"                          , GOSSIP_SENDER_MAIN, 2031);
	  				player->ADD_GOSSIP_ITEM( 1, "Pink"                            , GOSSIP_SENDER_MAIN, 2032);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                             , GOSSIP_SENDER_MAIN, 2033);
	  				player->ADD_GOSSIP_ITEM( 1, "Silver"                          , GOSSIP_SENDER_MAIN, 2034);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                          , GOSSIP_SENDER_MAIN, 2035);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	      , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	      , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2022: //Horde
            					CreatePet(player, _creature, 51673);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2023: //Horde Black
            					CreatePet(player, _creature, 50154);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2024: //Horde Blue
            					CreatePet(player, _creature, 51404);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2025: //Horde Green
            					CreatePet(player, _creature, 51402);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2026: //Horde Red
            					CreatePet(player, _creature, 51401);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2027: //Beiqe
            					CreatePet(player, _creature, 3125);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2028: //Black
            					CreatePet(player, _creature, 8301);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2029: //Blue
            					CreatePet(player, _creature, 48130);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2030: //Dark Iron
            					CreatePet(player, _creature, 44599);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2031: //Golden
            					CreatePet(player, _creature, 40656);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2032: //Pink
            					CreatePet(player, _creature, 40717);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2033: //Red
            					CreatePet(player, _creature, 7078);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2034: //Silver
            					CreatePet(player, _creature, 11737);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        				case 2035: //Yellow
            					CreatePet(player, _creature, 3126);
                  				_creature->MonsterWhisper(TEXT_SCORPID, player->GetGUID());
           					break;

        		 	case 203: //Serpent
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                           , GOSSIP_SENDER_MAIN, 2036);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"             		     , GOSSIP_SENDER_MAIN, 2037);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"             		     , GOSSIP_SENDER_MAIN, 2038);
	  				player->ADD_GOSSIP_ITEM( 1, "Olive"                          , GOSSIP_SENDER_MAIN, 2039);
	  				player->ADD_GOSSIP_ITEM( 1, "Purple"                         , GOSSIP_SENDER_MAIN, 2040);
	  				player->ADD_GOSSIP_ITEM( 1, "Violet"                         , GOSSIP_SENDER_MAIN, 2041);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                          , GOSSIP_SENDER_MAIN, 2042);
	  				player->ADD_GOSSIP_ITEM( 1, "Ruby"                  	     , GOSSIP_SENDER_MAIN, 2043);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2036: //Blue
            					CreatePet(player, _creature, 5048);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2037: //Brown
            					CreatePet(player, _creature, 40657);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2038: //Green
            					CreatePet(player, _creature, 5225);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2039: //Olive
            					CreatePet(player, _creature, 48131);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2040: //Purple
            					CreatePet(player, _creature, 5755);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2041: //Violet
            					CreatePet(player, _creature, 48132);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2042: //White
            					CreatePet(player, _creature, 19784);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        				case 2043: //Ruby
            					CreatePet(player, _creature, 44594);
                  				_creature->MonsterWhisper(TEXT_SERPENT, player->GetGUID());
           					break;

        		 	case 204: //Spider I
	  				player->ADD_GOSSIP_ITEM( 1, "Bone Black"                     , GOSSIP_SENDER_MAIN, 2044);
	  				player->ADD_GOSSIP_ITEM( 1, "Bone Brown"             	     , GOSSIP_SENDER_MAIN, 2045);
	  				player->ADD_GOSSIP_ITEM( 1, "Bone Gray"             	     , GOSSIP_SENDER_MAIN, 2046);
	  				player->ADD_GOSSIP_ITEM( 1, "Bone Ochre"                     , GOSSIP_SENDER_MAIN, 2047);
	  				player->ADD_GOSSIP_ITEM( 1, "Bone Slate"                     , GOSSIP_SENDER_MAIN, 2048);
	  				player->ADD_GOSSIP_ITEM( 1, "Giant"                          , GOSSIP_SENDER_MAIN, 2049);
	  				player->ADD_GOSSIP_ITEM( 1, "Giant Black"                    , GOSSIP_SENDER_MAIN, 2050);
	  				player->ADD_GOSSIP_ITEM( 1, "Giant Orange"                   , GOSSIP_SENDER_MAIN, 2051);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Blood"                     , GOSSIP_SENDER_MAIN, 2052);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Cave"                      , GOSSIP_SENDER_MAIN, 2053);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Crystal"                   , GOSSIP_SENDER_MAIN, 2054);
					player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"      	     	     , GOSSIP_SENDER_MAIN, 205);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2044: //Bone Black
            					CreatePet(player, _creature, 42337);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2045: //Bone Brown
            					CreatePet(player, _creature, 47205);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2046: //Bone Gray
            					CreatePet(player, _creature, 30206);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2047: //Bone Ochre
            					CreatePet(player, _creature, 46508);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2048: //Bone Slate
            					CreatePet(player, _creature, 48268);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2049: //Giant
            					CreatePet(player, _creature, 17522);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2050: //Giant Black
            					CreatePet(player, _creature, 47115);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2051: //Giant Orange
            					CreatePet(player, _creature, 18647);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2052: //Mine Blood
            					CreatePet(player, _creature, 471);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2053: //Mine Cave
            					CreatePet(player, _creature, 16352);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2054: //Mine Crystal
            					CreatePet(player, _creature, 18466);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;


        		 	case 205: //Spider II
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Green"                     , GOSSIP_SENDER_MAIN, 2055);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Jungle"             	     , GOSSIP_SENDER_MAIN, 2056);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Olive"             	     , GOSSIP_SENDER_MAIN, 2057);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Steel"                     , GOSSIP_SENDER_MAIN, 2058);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Violet"                    , GOSSIP_SENDER_MAIN, 2059);
	  				player->ADD_GOSSIP_ITEM( 1, "Mine Wetlands"                  , GOSSIP_SENDER_MAIN, 2060);
	  				player->ADD_GOSSIP_ITEM( 1, "Tarantula Brown"                , GOSSIP_SENDER_MAIN, 2061);
	  				player->ADD_GOSSIP_ITEM( 1, "Tarantula Green"          	     , GOSSIP_SENDER_MAIN, 2062);
	  				player->ADD_GOSSIP_ITEM( 1, "Tarantula Gray"           	     , GOSSIP_SENDER_MAIN, 2063);
	  				player->ADD_GOSSIP_ITEM( 1, "Tarantula Magma"          	     , GOSSIP_SENDER_MAIN, 2064);
	  				player->ADD_GOSSIP_ITEM( 1, "Tarantula Orange"         	     , GOSSIP_SENDER_MAIN, 2065);
					player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"      	     	     , GOSSIP_SENDER_MAIN, 204);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	     , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2055: //Mine Green
            					CreatePet(player, _creature, 4379);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2056: //Mine Jungle
            					CreatePet(player, _creature, 45227);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2057: //Mine Olive
            					CreatePet(player, _creature, 15975);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2058: //Mine Steel
            					CreatePet(player, _creature, 45582);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2059: //Mine Violet
            					CreatePet(player, _creature, 49346);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2060: //Mine Wetlands
            					CreatePet(player, _creature, 1111);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2061: //Tarantula Brown
            					CreatePet(player, _creature, 8933);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2062: //Tarantula Green
            					CreatePet(player, _creature, 4007);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2063: //Tarantula Gray
            					CreatePet(player, _creature, 18983);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2064: //Tarantula Magma
            					CreatePet(player, _creature, 5858);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        				case 2065: //Tarantula Orange
            					CreatePet(player, _creature, 442);
                  				_creature->MonsterWhisper(TEXT_SPIDER, player->GetGUID());
           					break;

        		 	case 206: //Tallstrider
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"                       , GOSSIP_SENDER_MAIN, 2066);
	  				player->ADD_GOSSIP_ITEM( 1, "Gray"             	         , GOSSIP_SENDER_MAIN, 2067);
	  				player->ADD_GOSSIP_ITEM( 1, "Ivory"             	         , GOSSIP_SENDER_MAIN, 2068);
	  				player->ADD_GOSSIP_ITEM( 1, "Pink"                        , GOSSIP_SENDER_MAIN, 2069);
	  				player->ADD_GOSSIP_ITEM( 1, "Purple"                      , GOSSIP_SENDER_MAIN, 2070);
	  				player->ADD_GOSSIP_ITEM( 1, "Turquoise"                   , GOSSIP_SENDER_MAIN, 2071);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	  , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	  , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2066: //Brown
            					CreatePet(player, _creature, 32997);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;

        				case 2067: //Gray
            					CreatePet(player, _creature, 2956);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;


        				case 2068: //Ivory
            					CreatePet(player, _creature, 2957);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;


        				case 2069: //Pink
            					CreatePet(player, _creature, 37091);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;


        				case 2070: //Purple
            					CreatePet(player, _creature, 22807);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;


        				case 2071: //Turquoise
            					CreatePet(player, _creature, 2172);
                  				_creature->MonsterWhisper(TEXT_TALLSTRIDER, player->GetGUID());
           					break;


     		 		case 207: //Turtle
	  				player->ADD_GOSSIP_ITEM( 1, "Sea"                         , GOSSIP_SENDER_MAIN, 2072);
	  				player->ADD_GOSSIP_ITEM( 1, "Sea Blue"             	  , GOSSIP_SENDER_MAIN, 2073);
	  				player->ADD_GOSSIP_ITEM( 1, "Sea Gray"             	  , GOSSIP_SENDER_MAIN, 2074);
	  				player->ADD_GOSSIP_ITEM( 1, "Sea Red"                     , GOSSIP_SENDER_MAIN, 2075);
	  				player->ADD_GOSSIP_ITEM( 1, "Sea White"                   , GOSSIP_SENDER_MAIN, 2076);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	  , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	  , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2072: //Sea
            					CreatePet(player, _creature, 4824);
                  				_creature->MonsterWhisper(TEXT_TURTLE, player->GetGUID());
           					break;

        				case 2073: //Sea Blue
            					CreatePet(player, _creature, 7977);
                  				_creature->MonsterWhisper(TEXT_TURTLE, player->GetGUID());
           					break;

        				case 2074: //Sea Gray
            					CreatePet(player, _creature, 8213);
                  				_creature->MonsterWhisper(TEXT_TURTLE, player->GetGUID());
           					break;

        				case 2075: //Sea Red
            					CreatePet(player, _creature, 4825);
                  				_creature->MonsterWhisper(TEXT_TURTLE, player->GetGUID());
           					break;

        				case 2076: //Sea White
            					CreatePet(player, _creature, 36732);
                  				_creature->MonsterWhisper(TEXT_TURTLE, player->GetGUID());
           					break;


     		 		case 208: //Wind Serpent
	  				player->ADD_GOSSIP_ITEM( 1, "Wind Serpent"                , GOSSIP_SENDER_MAIN, 2077);
	  				player->ADD_GOSSIP_ITEM( 1, "Black"             	  	  , GOSSIP_SENDER_MAIN, 2078);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"             	  	  , GOSSIP_SENDER_MAIN, 2079);
	  				player->ADD_GOSSIP_ITEM( 1, "Orange"                      , GOSSIP_SENDER_MAIN, 2080);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland"                     , GOSSIP_SENDER_MAIN, 2081);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Dark"                , GOSSIP_SENDER_MAIN, 2082);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Green"               , GOSSIP_SENDER_MAIN, 2083);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Blue"             	  , GOSSIP_SENDER_MAIN, 2084);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Violet"              , GOSSIP_SENDER_MAIN, 2085);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Red"                 , GOSSIP_SENDER_MAIN, 2086);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland White"               , GOSSIP_SENDER_MAIN, 2087);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	  , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	  , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2077: //Wind Serpent
            					CreatePet(player, _creature, 3630);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2078: //Black
            					CreatePet(player, _creature, 5056);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2079: //Green
            					CreatePet(player, _creature, 3631);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2080: //Orange
            					CreatePet(player, _creature, 20797);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2081: //Outland
            					CreatePet(player, _creature, 20749);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2082: //Outland Dark
            					CreatePet(player, _creature, 20673);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2083: //Outland Green
            					CreatePet(player, _creature, 21123);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2084: //Outland Blue
            					CreatePet(player, _creature, 19428);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2085: //Outland Violet
            					CreatePet(player, _creature, 28477);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2086: //Outland Red
            					CreatePet(player, _creature, 5834);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

        				case 2087: //Outland White
            					CreatePet(player, _creature, 5349);
                  				_creature->MonsterWhisper(TEXT_WIND_SERPENT, player->GetGUID());
           					break;

     		 		case 209: //Wolf I
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Blue Brown"             , GOSSIP_SENDER_MAIN, 2088);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Brown"             	  , GOSSIP_SENDER_MAIN, 2089);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Dark Black"             , GOSSIP_SENDER_MAIN, 2090);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Dark Brown"             , GOSSIP_SENDER_MAIN, 2091);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Dark Gray"              , GOSSIP_SENDER_MAIN, 2092);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Light Blue"             , GOSSIP_SENDER_MAIN, 2093);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Light Gray"             , GOSSIP_SENDER_MAIN, 2094);
	  				player->ADD_GOSSIP_ITEM( 1, "Dire Reddish Brown"          , GOSSIP_SENDER_MAIN, 2095);
	  				player->ADD_GOSSIP_ITEM( 1, "PvP Dire Dark Black"         , GOSSIP_SENDER_MAIN, 2096);
					player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"      	     	  , GOSSIP_SENDER_MAIN, 210);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	  , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	  , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2088: //Dire Blue Brown
            					CreatePet(player, _creature, 18670);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2089: //Dire Brown
            					CreatePet(player, _creature, 19458);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2090: //Dire Dark Black
            					CreatePet(player, _creature, 628);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2091: //Dire Dark Brown
            					CreatePet(player, _creature, 10077);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2092: //Dire Dark Gray
            					CreatePet(player, _creature, 8211);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2093: //Dire Light Blue
            					CreatePet(player, _creature, 14339);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2094: //Dire Light Gray
            					CreatePet(player, _creature, 12431);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2095: //Dire Reddish Brown
            					CreatePet(player, _creature, 2753);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2096: //PvP Dire Dark Black
            					CreatePet(player, _creature, 17280);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

     		 		case 210: //Wolf II
	  				player->ADD_GOSSIP_ITEM( 1, "Arctic"                      , GOSSIP_SENDER_MAIN, 2100);
	  				player->ADD_GOSSIP_ITEM( 1, "Black"             	  	  , GOSSIP_SENDER_MAIN, 2101);
	  				player->ADD_GOSSIP_ITEM( 1, "Coyote"             	  , GOSSIP_SENDER_MAIN, 2102);
	  				player->ADD_GOSSIP_ITEM( 1, "Diseased"                    , GOSSIP_SENDER_MAIN, 2103);
	  				player->ADD_GOSSIP_ITEM( 1, "Timber"                      , GOSSIP_SENDER_MAIN, 2104);
	  				player->ADD_GOSSIP_ITEM( 1, "Worg Black"                  , GOSSIP_SENDER_MAIN, 2105);
	  				player->ADD_GOSSIP_ITEM( 1, "Worg Brown"                  , GOSSIP_SENDER_MAIN, 2106);
	  				player->ADD_GOSSIP_ITEM( 1, "Worg Gray"                   , GOSSIP_SENDER_MAIN, 2107);
	  				player->ADD_GOSSIP_ITEM( 1, "Worg White"                  , GOSSIP_SENDER_MAIN, 2108);
					player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"      	     	  , GOSSIP_SENDER_MAIN, 209);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     	  , GOSSIP_SENDER_MAIN, 2);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	  , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 2100: //Arctic
            					CreatePet(player, _creature, 43704);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2101: //Black
            					CreatePet(player, _creature, 43340);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2102: //Coyote
            					CreatePet(player, _creature, 20058);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2103: //Diseased
            					CreatePet(player, _creature, 69);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2104: //Timber
            					CreatePet(player, _creature, 1922);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2105: //Worg Black
            					CreatePet(player, _creature, 47769);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2106: //Worg Brown
            					CreatePet(player, _creature, 24516);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2107: //Worg Gray
            					CreatePet(player, _creature, 30164);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        				case 2108: //Worg White
            					CreatePet(player, _creature, 26672);
                  				_creature->MonsterWhisper(TEXT_WOLF, player->GetGUID());
           					break;

        		 case 3: //The Burning Crusader
	  			player->ADD_GOSSIP_ITEM( 1, "Dragonhawk"                     , GOSSIP_SENDER_MAIN, 300);
	  			player->ADD_GOSSIP_ITEM( 1, "Nether Ray"                     , GOSSIP_SENDER_MAIN, 301);
	  			player->ADD_GOSSIP_ITEM( 1, "Ravager"                        , GOSSIP_SENDER_MAIN, 302);
	  			player->ADD_GOSSIP_ITEM( 1, "Sporebat"                       , GOSSIP_SENDER_MAIN, 303);
	  			player->ADD_GOSSIP_ITEM( 1, "Warp Stalker"                   , GOSSIP_SENDER_MAIN, 304);
				player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 	break;

     		 		case 300: //Dragonhawk
	  				player->ADD_GOSSIP_ITEM( 1, "Dragonhawk"              , GOSSIP_SENDER_MAIN, 3000);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"             	     , GOSSIP_SENDER_MAIN, 3001);
	  				player->ADD_GOSSIP_ITEM( 1, "Amani"             	     , GOSSIP_SENDER_MAIN, 3002);
	  				player->ADD_GOSSIP_ITEM( 1, "Pink"                    , GOSSIP_SENDER_MAIN, 3003);
	  				player->ADD_GOSSIP_ITEM( 1, "Violet"                  , GOSSIP_SENDER_MAIN, 3004);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                     , GOSSIP_SENDER_MAIN, 3005);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 3);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 3000: //Dragonhawk
            					CreatePet(player, _creature, 15649);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

        				case 3001: //Red
            					CreatePet(player, _creature, 15650);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

        				case 3002: //Amani
            					CreatePet(player, _creature, 23834);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

        				case 3003: //Pink
            					CreatePet(player, _creature, 20502);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

        				case 3004: //Violet
            					CreatePet(player, _creature, 25867);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

        				case 3005: //Red
            					CreatePet(player, _creature, 18155);
                  				_creature->MonsterWhisper(TEXT_DRAGONHAWK, player->GetGUID());
           					break;

     		 		case 301: //Nether Ray
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                   , GOSSIP_SENDER_MAIN, 3010);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"             	     , GOSSIP_SENDER_MAIN, 3011);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"             	     , GOSSIP_SENDER_MAIN, 3012);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                     , GOSSIP_SENDER_MAIN, 3013);
	  				player->ADD_GOSSIP_ITEM( 1, "Wyrm"                    , GOSSIP_SENDER_MAIN, 3014);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 3);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 3010: //Black
            					CreatePet(player, _creature, 1825);
                  				_creature->MonsterWhisper(TEXT_NETHER_RAY, player->GetGUID());
           					break;

        				case 3011: //Blue
            					CreatePet(player, _creature, 20196);
                  				_creature->MonsterWhisper(TEXT_NETHER_RAY, player->GetGUID());
           					break;

        				case 3012: //Green
            					CreatePet(player, _creature, 18130);
                  				_creature->MonsterWhisper(TEXT_NETHER_RAY, player->GetGUID());
           					break;

        				case 3013: //Red
            					CreatePet(player, _creature, 21901);
                  				_creature->MonsterWhisper(TEXT_NETHER_RAY, player->GetGUID());
           					break;

        				case 3014: //Wyrm
            					CreatePet(player, _creature, 18131);
                  				_creature->MonsterWhisper(TEXT_NETHER_RAY, player->GetGUID());
           					break;

     		 		case 302: //Ravager
	  				player->ADD_GOSSIP_ITEM( 1, "Elite Green"             , GOSSIP_SENDER_MAIN, 3020);
	  				player->ADD_GOSSIP_ITEM( 1, "Elite Orange"            , GOSSIP_SENDER_MAIN, 3021);
	  				player->ADD_GOSSIP_ITEM( 1, "Elite Purple"            , GOSSIP_SENDER_MAIN, 3022);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Green"           , GOSSIP_SENDER_MAIN, 3023);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Orange"          , GOSSIP_SENDER_MAIN, 3024);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Purple"          , GOSSIP_SENDER_MAIN, 3025);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var I"           , GOSSIP_SENDER_MAIN, 3026);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var II"          , GOSSIP_SENDER_MAIN, 3027);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var III"         , GOSSIP_SENDER_MAIN, 3028);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var IV"          , GOSSIP_SENDER_MAIN, 3029);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var V"           , GOSSIP_SENDER_MAIN, 3030);
	  				player->ADD_GOSSIP_ITEM( 1, "Crawler Var VI"          , GOSSIP_SENDER_MAIN, 3031);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 3);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 3020: //Elite Green
            					CreatePet(player, _creature, 16934);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3021: //Elite Orange
            					CreatePet(player, _creature, 16933);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3022: //Elite Purple
            					CreatePet(player, _creature, 19349);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3023: //Crawler Green
            					CreatePet(player, _creature, 17527);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3024: //Crawler Orange
            					CreatePet(player, _creature, 17119);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3025: //Crawler Purple
            					CreatePet(player, _creature, 22123);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3026: //Crawler Var I
            					CreatePet(player, _creature, 16932);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3027: //Crawler Var II
            					CreatePet(player, _creature, 21423);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3028: //Crawler Var III
            					CreatePet(player, _creature, 19189);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3029: //Crawler Var IV
            					CreatePet(player, _creature, 23326);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3030: //Crawler Var V
            					CreatePet(player, _creature, 19350);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

        				case 3031: //Crawler Var VI
            					CreatePet(player, _creature, 17525);
                  				_creature->MonsterWhisper(TEXT_RAVAGER, player->GetGUID());
           					break;

     		 		case 303: //Sporebat
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                    , GOSSIP_SENDER_MAIN, 3032);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"             	     , GOSSIP_SENDER_MAIN, 3033);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                  , GOSSIP_SENDER_MAIN, 3034);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 3);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 3032: //Blue
            					CreatePet(player, _creature, 18128);
                  				_creature->MonsterWhisper(TEXT_SPOREBAT, player->GetGUID());
           					break;

        				case 3033: //Green
            					CreatePet(player, _creature, 18129);
                  				_creature->MonsterWhisper(TEXT_SPOREBAT, player->GetGUID());
           					break;

        				case 3034: //Yellow
            					CreatePet(player, _creature, 18280);
                  				_creature->MonsterWhisper(TEXT_SPOREBAT, player->GetGUID());
           					break;

     		 		case 304: //Warp Stalker
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                   , GOSSIP_SENDER_MAIN, 3040);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"             	     , GOSSIP_SENDER_MAIN, 3041);
	  				player->ADD_GOSSIP_ITEM( 1, "Turquiose"               , GOSSIP_SENDER_MAIN, 3042);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                   , GOSSIP_SENDER_MAIN, 3043);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 3);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 3040: //Black
            					CreatePet(player, _creature, 23163);
                  				_creature->MonsterWhisper(TEXT_WARP_STALKER, player->GetGUID());
           					break;

        				case 3041: //Green
            					CreatePet(player, _creature, 50313);
                  				_creature->MonsterWhisper(TEXT_WARP_STALKER, player->GetGUID());
           					break;

        				case 3042: //Turquiose
            					CreatePet(player, _creature, 18884);
                  				_creature->MonsterWhisper(TEXT_WARP_STALKER, player->GetGUID());
           					break;

        				case 3043: //White
            					CreatePet(player, _creature, 23219);
                  				_creature->MonsterWhisper(TEXT_WARP_STALKER, player->GetGUID());
           					break;

        		 case 4: //Wrath of the Lich King
	  			player->ADD_GOSSIP_ITEM( 1, "Chimaera"                       , GOSSIP_SENDER_MAIN, 400);
	  			player->ADD_GOSSIP_ITEM( 1, "Core Hound"                     , GOSSIP_SENDER_MAIN, 401);
	  			player->ADD_GOSSIP_ITEM( 1, "Devilsau"                       , GOSSIP_SENDER_MAIN, 402);
	  			player->ADD_GOSSIP_ITEM( 1, "Moth"                           , GOSSIP_SENDER_MAIN, 403);
	  			player->ADD_GOSSIP_ITEM( 1, "Rhino"                          , GOSSIP_SENDER_MAIN, 404);
	  			player->ADD_GOSSIP_ITEM( 1, "Silithid"                       , GOSSIP_SENDER_MAIN, 405);
	  			player->ADD_GOSSIP_ITEM( 1, "Spirit Beast"                   , GOSSIP_SENDER_MAIN, 406);
	  			player->ADD_GOSSIP_ITEM( 1, "Wasp"                           , GOSSIP_SENDER_MAIN, 407);
	  			player->ADD_GOSSIP_ITEM( 1, "Worm"                           , GOSSIP_SENDER_MAIN, 409);
				player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"      	     , GOSSIP_SENDER_MAIN, 6);

  				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 	break;

     		 		case 400: //Chimaera
	  				player->ADD_GOSSIP_ITEM( 1, "Chimaera"                , GOSSIP_SENDER_MAIN, 4000);
	  				player->ADD_GOSSIP_ITEM( 1, "Beige"             	     , GOSSIP_SENDER_MAIN, 4001);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                    , GOSSIP_SENDER_MAIN, 4002);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"                   , GOSSIP_SENDER_MAIN, 4003);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Green"           , GOSSIP_SENDER_MAIN, 4004);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Purple"          , GOSSIP_SENDER_MAIN, 4005);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland White"           , GOSSIP_SENDER_MAIN, 4006);
	  				player->ADD_GOSSIP_ITEM( 1, "Outland Yellow"          , GOSSIP_SENDER_MAIN, 4007);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4000: //Chimaera
            					CreatePet(player, _creature, 36304);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4001: //Beige
            					CreatePet(player, _creature, 8764);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4002: //Blue
            					CreatePet(player, _creature, 10807);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4003: //Green
            					CreatePet(player, _creature, 8660);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4004: //Outland Green
            					CreatePet(player, _creature, 21879);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4005: //Outland Purple
            					CreatePet(player, _creature, 20932);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4006: //Outland White
            					CreatePet(player, _creature, 11497);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

        				case 4007: //Outland Yellow
            					CreatePet(player, _creature, 21033);
                  				_creature->MonsterWhisper(TEXT_CHIMAERA, player->GetGUID());
           					break;

     		 		case 401: //Core Hound
	  				player->ADD_GOSSIP_ITEM( 1, "Green Black"             , GOSSIP_SENDER_MAIN, 4010);
	  				player->ADD_GOSSIP_ITEM( 1, "Red Black"               , GOSSIP_SENDER_MAIN, 4011);
	  				player->ADD_GOSSIP_ITEM( 1, "Nian"                    , GOSSIP_SENDER_MAIN, 4012);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4010: //Green Black
            					CreatePet(player, _creature, 41166);
                  				_creature->MonsterWhisper(TEXT_CORE_HOUND, player->GetGUID());
           					break;

        				case 4011: //Red Black
            					CreatePet(player, _creature, 11673);
                  				_creature->MonsterWhisper(TEXT_CORE_HOUND, player->GetGUID());
           					break;

        				case 4012: //Nian
            					CreatePet(player, _creature, 17447);
                  				_creature->MonsterWhisper(TEXT_CORE_HOUND, player->GetGUID());
           					break;

     		 		case 402: //Devilsaur
	  				player->ADD_GOSSIP_ITEM( 1, "Black"                   , GOSSIP_SENDER_MAIN, 4020);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"                   , GOSSIP_SENDER_MAIN, 4021);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                     , GOSSIP_SENDER_MAIN, 4022);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                   , GOSSIP_SENDER_MAIN, 4023);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4020: //Black
            					CreatePet(player, _creature, 6499);
                  				_creature->MonsterWhisper(TEXT_DEVILSAUR, player->GetGUID());
           					break;

        				case 4021: //Green
            					CreatePet(player, _creature, 32485);
                  				_creature->MonsterWhisper(TEXT_DEVILSAUR, player->GetGUID());
           					break;

        				case 4022: //Red
            					CreatePet(player, _creature, 38346);
                  				_creature->MonsterWhisper(TEXT_DEVILSAUR, player->GetGUID());
           					break;

        				case 4023: //White
            					CreatePet(player, _creature, 6498);
                  				_creature->MonsterWhisper(TEXT_DEVILSAUR, player->GetGUID());
           					break;

     		 		case 403: //Moth
	  				player->ADD_GOSSIP_ITEM( 1, "Beige"                   , GOSSIP_SENDER_MAIN, 4030);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                    , GOSSIP_SENDER_MAIN, 4031);
	  				player->ADD_GOSSIP_ITEM( 1, "Gray"                    , GOSSIP_SENDER_MAIN, 4032);
	  				player->ADD_GOSSIP_ITEM( 1, "Red"                     , GOSSIP_SENDER_MAIN, 4033);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                   , GOSSIP_SENDER_MAIN, 4034);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"                  , GOSSIP_SENDER_MAIN, 4035);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4030: //Beige
            					CreatePet(player, _creature, 18468);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

        				case 4031: //Blue
            					CreatePet(player, _creature, 17349);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

        				case 4032: //Gray
            					CreatePet(player, _creature, 16520);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

        				case 4033: //Red
            					CreatePet(player, _creature, 25464);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

        				case 4034: //White
            					CreatePet(player, _creature, 25498);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

        				case 4035: //Yellow
            					CreatePet(player, _creature, 27421);
                  				_creature->MonsterWhisper(TEXT_MOTH, player->GetGUID());
           					break;

     		 		case 404: //Rhino
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"                    , GOSSIP_SENDER_MAIN, 4040);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"                   , GOSSIP_SENDER_MAIN, 4041);
	  				player->ADD_GOSSIP_ITEM( 1, "Slate"                   , GOSSIP_SENDER_MAIN, 4042);
	  				player->ADD_GOSSIP_ITEM( 1, "White"                   , GOSSIP_SENDER_MAIN, 4043);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4040: //Blue
            					CreatePet(player, _creature, 29931);
                  				_creature->MonsterWhisper(TEXT_RHINO, player->GetGUID());
           					break;

        				case 4041: //Brown
            					CreatePet(player, _creature, 25487);
                  				_creature->MonsterWhisper(TEXT_RHINO, player->GetGUID());
           					break;

        				case 4042: //Slate
            					CreatePet(player, _creature, 25489);
                  				_creature->MonsterWhisper(TEXT_RHINO, player->GetGUID());
           					break;

        				case 4043: //White
            					CreatePet(player, _creature, 30445);
                  				_creature->MonsterWhisper(TEXT_RHINO, player->GetGUID());
           					break;

     		 		case 405: //Silithid
	  				player->ADD_GOSSIP_ITEM( 1, "Aurokk"                  , GOSSIP_SENDER_MAIN, 4050);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Boss"               , GOSSIP_SENDER_MAIN, 4051);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Boss Blue"          , GOSSIP_SENDER_MAIN, 4052);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Boss Different"     , GOSSIP_SENDER_MAIN, 4053);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Blue"               , GOSSIP_SENDER_MAIN, 4054);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Golden"             , GOSSIP_SENDER_MAIN, 4055);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Tan"                , GOSSIP_SENDER_MAIN, 4056);
	  				player->ADD_GOSSIP_ITEM( 1, "Tank Violet"             , GOSSIP_SENDER_MAIN, 4057);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4050: //Aurokk
            					CreatePet(player, _creature, 3252);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4051: //Tank Boss
            					CreatePet(player, _creature, 47386);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4052: //Tank Boss Blue
            					CreatePet(player, _creature, 47387);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4053: //Tank Boss Different
            					CreatePet(player, _creature, 38305);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4054: //Tank Blue
            					CreatePet(player, _creature, 8205);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4055: //Tank Golden
            					CreatePet(player, _creature, 6582);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4056: //Tank Tan
            					CreatePet(player, _creature, 4132);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

        				case 4057: //Tank Violet
            					CreatePet(player, _creature, 5460);
                  				_creature->MonsterWhisper(TEXT_SILITHID, player->GetGUID());
           					break;

     		 		case 406: //Spirit Beast
	  				player->ADD_GOSSIP_ITEM( 1, "Bear Spectral"           , GOSSIP_SENDER_MAIN, 4060);
	  				player->ADD_GOSSIP_ITEM( 1, "Spectral Saber Worg"     , GOSSIP_SENDER_MAIN, 4061);
	  				player->ADD_GOSSIP_ITEM( 1, "Spectral Tiger Purple"   , GOSSIP_SENDER_MAIN, 4062);
	  				player->ADD_GOSSIP_ITEM( 1, "Wolf Ghost"              , GOSSIP_SENDER_MAIN, 4063);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4060: //Bear Spectral
            					CreatePet(player, _creature, 38453);
                  				_creature->MonsterWhisper(TEXT_SPIRIT_BEAST, player->GetGUID());
           					break;

        				case 4061: //Spectral Saber Worg
            					CreatePet(player, _creature, 35189);
                  				_creature->MonsterWhisper(TEXT_SPIRIT_BEAST, player->GetGUID());
           					break;

        				case 4062: //Spectral Tiger Purple
            					CreatePet(player, _creature, 33776);
                  				_creature->MonsterWhisper(TEXT_SPIRIT_BEAST, player->GetGUID());
           					break;

        				case 4063: //Wolf Ghost
            					CreatePet(player, _creature, 50138);
                  				_creature->MonsterWhisper(TEXT_SPIRIT_BEAST, player->GetGUID());
           					break;

     		 		case 407: //Wasp I
	  				player->ADD_GOSSIP_ITEM( 1, "Fire Fly Black"          , GOSSIP_SENDER_MAIN, 4070);
	  				player->ADD_GOSSIP_ITEM( 1, "Fire Fly Blue"     	     , GOSSIP_SENDER_MAIN, 4071);
	  				player->ADD_GOSSIP_ITEM( 1, "Fire Fly Green"          , GOSSIP_SENDER_MAIN, 4072);
	  				player->ADD_GOSSIP_ITEM( 1, "Fire Fly Purple"         , GOSSIP_SENDER_MAIN, 4073);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Black"          , GOSSIP_SENDER_MAIN, 4074);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Blue"           , GOSSIP_SENDER_MAIN, 4075);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Orange"         , GOSSIP_SENDER_MAIN, 4076);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Purple"         , GOSSIP_SENDER_MAIN, 4077);
					player->ADD_GOSSIP_ITEM( 7, "[Page II] ->"      	     , GOSSIP_SENDER_MAIN, 408);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4070: //Fire Fly Black
            					CreatePet(player, _creature, 18283);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4071: //Fire Fly Blue
            					CreatePet(player, _creature, 20197);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4072: //Fire Fly Green
            					CreatePet(player, _creature, 20198);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4073: //Fire Fly Purple
            					CreatePet(player, _creature, 18133);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4074: //Silithid Black
            					CreatePet(player, _creature, 14475);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4075: //Silithid Blue
            					CreatePet(player, _creature, 5450);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4076: //Silithid Orange
            					CreatePet(player, _creature, 14474);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4077: //Silithid Purple
            					CreatePet(player, _creature, 5456);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

     		 		case 408: //Wasp II
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Tan"            , GOSSIP_SENDER_MAIN, 4078);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid White"          , GOSSIP_SENDER_MAIN, 4079);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Yellow"         , GOSSIP_SENDER_MAIN, 4080);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Wasp Boss I"    , GOSSIP_SENDER_MAIN, 4081);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Wasp Boss II"   , GOSSIP_SENDER_MAIN, 4082);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Wasp Boss III"  , GOSSIP_SENDER_MAIN, 4083);
	  				player->ADD_GOSSIP_ITEM( 1, "Silithid Wasp Boss IV"   , GOSSIP_SENDER_MAIN, 4084);
					player->ADD_GOSSIP_ITEM( 7, "<- [Page I]"      	     , GOSSIP_SENDER_MAIN, 407);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	     , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"          , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4078: //Silithid Tan
            					CreatePet(player, _creature, 48179);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4079: //Silithid White
            					CreatePet(player, _creature, 15288);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4080: //Silithid Yellow
            					CreatePet(player, _creature, 6554);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4081: //Silithid Wasp Boss I
            					CreatePet(player, _creature, 6554);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4082: //Silithid Wasp Boss II
            					CreatePet(player, _creature, 28086);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4083: //Silithid Wasp Boss III
            					CreatePet(player, _creature, 28085);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

        				case 4084: //Silithid Wasp Boss IV
            					CreatePet(player, _creature, 51712);
                  				_creature->MonsterWhisper(TEXT_WASP, player->GetGUID());
           					break;

     		 		case 409: //Worm
	  				player->ADD_GOSSIP_ITEM( 1, "Jormungar Larva Blue"      , GOSSIP_SENDER_MAIN, 4085);
	  				player->ADD_GOSSIP_ITEM( 1, "Jormungar Larva Green"     , GOSSIP_SENDER_MAIN, 4086);
	  				player->ADD_GOSSIP_ITEM( 1, "Jormungar Larva Pale"      , GOSSIP_SENDER_MAIN, 4087);
	  				player->ADD_GOSSIP_ITEM( 1, "Jormungar Larva White"     , GOSSIP_SENDER_MAIN, 4088);
	  				player->ADD_GOSSIP_ITEM( 1, "Blue"   			, GOSSIP_SENDER_MAIN, 4089);
	  				player->ADD_GOSSIP_ITEM( 1, "Brown"  			, GOSSIP_SENDER_MAIN, 4090);
	  				player->ADD_GOSSIP_ITEM( 1, "Gray"   			, GOSSIP_SENDER_MAIN, 4091);
	  				player->ADD_GOSSIP_ITEM( 1, "Green"   			, GOSSIP_SENDER_MAIN, 4092);
	  				player->ADD_GOSSIP_ITEM( 1, "Purple"   			, GOSSIP_SENDER_MAIN, 4093);
	  				player->ADD_GOSSIP_ITEM( 1, "White"   			, GOSSIP_SENDER_MAIN, 4094);
	  				player->ADD_GOSSIP_ITEM( 1, "Yellow"   			, GOSSIP_SENDER_MAIN, 4095);
					player->ADD_GOSSIP_ITEM( 7, "<- [Back]"      	       , GOSSIP_SENDER_MAIN, 4);
					player->ADD_GOSSIP_ITEM( 7, "<- [Main Menu]"            , GOSSIP_SENDER_MAIN, 6);

  					player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 		break;

        				case 4085: //Jormungar Larva Blue
            					CreatePet(player, _creature, 26293);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4086: //Jormungar Larva Green
            					CreatePet(player, _creature, 26358);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4087: //Jormungar Larva Pale
            					CreatePet(player, _creature, 30291);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4088: //Jormungar Larva White
            					CreatePet(player, _creature, 26359);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4089: //Blue
            					CreatePet(player, _creature, 11789);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4090: //Brown
            					CreatePet(player, _creature, 8925);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4091: //Gray
            					CreatePet(player, _creature, 11741);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4092: //Green
            					CreatePet(player, _creature, 11787);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4093: //Purple
            					CreatePet(player, _creature, 49347);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4094: //White
            					CreatePet(player, _creature, 14370);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        				case 4095: //Yellow
            					CreatePet(player, _creature, 14477);
                  				_creature->MonsterWhisper(TEXT_WORM, player->GetGUID());
           					break;

        		 case 6: //<- [Main Menu]
	  			player->ADD_GOSSIP_ITEM( 6, "Classic"                     , GOSSIP_SENDER_MAIN, 1);
	  			player->ADD_GOSSIP_ITEM( 6, "The Burning Crusader"        , GOSSIP_SENDER_MAIN, 3);
	  			player->ADD_GOSSIP_ITEM( 6, "Wrath of the Lich King"      , GOSSIP_SENDER_MAIN, 4);

  				player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, _creature->GetGUID());
			 	break;

			 default:
				 break;
    	 		 }
           	  }
	     	  return true;
	  }
};

void AddSC_npc_petmaster()
{
    new npc_petmaster();
}