#include "ScriptPCH.h"
#include "AzsharaPvP.h"

enum Texts
{
	TEXT_EVADE = -700000,
	TEXT_ENTER_COMBAT = -700001
};

enum Spells
{
	spell_1 = 59047,
	spell_2 = 46763,
	spell_3 = 26561,
	spell_4 = 65947,
	spell_5 = 54135
};

Position const MeleePos = {};

class AzsharaPvP_Melee : public CreatureScript
{
public:
	AzsharaPvP_Melee() : CreatureScript("AzsharaPvP_Melee") {}

	struct AzsharaPvP_MeleeAI : public ScriptedAI
	{
		AzsharaPvP_MeleeAI(Creature* c) : ScriptedAI(c) {}

		uint32 spell1;
		uint32 spell2;
		uint32 spell3;
		uint32 spell4;
		uint32 spell5;

		void Reset()
		{
			spell1 = 10000;
			spell2 = 40000;
			spell3 = urand(20000, 30000);
			spell4 = 60000;
			spell5 = 45000;
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
				DoCast(me, spell_2);
				spell2 = 40000;
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

			if(spell4 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_4);
				spell3 = 60000;
			}
			else
				spell4 -= uiDiff;

			if(spell5 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_5);
				spell5 = 45000;
			}
			else
				spell5 -= uiDiff;
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new AzsharaPvP_MeleeAI(creature);
	}
};

void AddSC_AzsharaPvP_Melee()
{
	new AzsharaPvP_Melee();
}