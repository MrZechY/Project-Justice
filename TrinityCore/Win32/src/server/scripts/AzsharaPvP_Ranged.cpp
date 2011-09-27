#include "ScriptPCH.h"
#include "AzsharaPvP.h"

enum Texts
{
	TEXT_EVADE = -700000,
	TEXT_ENTER_COMBAT = -700001
};

enum Spells
{
	spell_1 = 48876,
	spell_2 = 24687,
	spell_3 = 67979
};

Position const RangedPos = {};

class AzsharaPvP_Ranged : public CreatureScript
{
public:
	AzsharaPvP_Ranged() : CreatureScript("AzsharaPvP_Ranged") {}

	struct AzsharaPvP_RangedAI : public ScriptedAI
	{
		AzsharaPvP_RangedAI(Creature* c) : ScriptedAI(c) {}

		uint32 spell1;
		uint32 spell2;
		uint32 spell3;

		void Reset()
		{
			spell1 = 10000;
			spell2 = 20000;
			spell3 = urand(20000, 30000);
		}

		void EnterCombat(Unit* who)
		{
			DoScriptText(TEXT_ENTER_COMBAT, me, who);
		}

		void EnterEvadeMode()
		{
			DoScriptText(TEXT_EVADE, me);
		}

		void UpdateAI(const uint32 uiDiff)
		{
			if(spell1 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_1);
				spell1 = 10000;
			}
			else
				spell1 -= uiDiff;

			if(spell2 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_2);
				spell2 = 20000;
			}
			else
				spell2 -= uiDiff;

			if(spell3 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_3);
				spell3 = urand(20000, 30000);
			}
			else
				spell3 -= uiDiff;
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new AzsharaPvP_RangedAI(creature);
	}
};

void AddSC_AzsharaPvP_Ranged()
{
	new AzsharaPvP_Ranged();
}