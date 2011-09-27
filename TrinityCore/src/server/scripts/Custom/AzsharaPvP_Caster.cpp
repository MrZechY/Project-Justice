#include "ScriptPCH.h"
#include "AzsharaPvP.h"

enum Texts
{
	TEXT_EVADE = -700000,
	TEXT_ENTER_COMBAT = -700001
};

enum Spells
{
	spell_1 = 66011,
	spell_2 = 58054,
	spell_3 = 68010,
	spell_4 = 17286,
	spell_5 = 52442,
	spell_6 = 63148,
	spell_7 = 58822
};

Position const CasterPos = {};

class AzsharaPvP_Caster : public CreatureScript
{
public:
	AzsharaPvP_Caster() : CreatureScript("AzsharaPvP_Caster") {}

	struct AzsharaPvP_CasterAI : public ScriptedAI
	{
		AzsharaPvP_CasterAI(Creature* c) : ScriptedAI(c) {}

		uint32 spell1;
		uint32 spell2;
		uint32 spell3;
		uint32 spell4;
		uint32 spell5;
		uint32 spell6;
		uint32 spell7;

		void Reset()
		{
			spell1 = 30000;
			spell3 = urand(20000, 30000);
			spell4 = 20000;
			spell6 = urand(40000, 60000);
			spell7 = 1000;
			spell2 = 0;
			spell5 = 0;
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
				spell1 = 30000;
			}
			else
				spell1 -= uiDiff;

			if(spell2 <= uiDiff)
				DoCast(me, spell_2);

			if(spell3 <= uiDiff)
			{
				DoCast(me, spell_3);
				spell3 = urand(20000, 30000);
			}
			else
				spell3 -= uiDiff;

			if(spell4 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_4);
				spell4 = 20000;
			}
			else
				spell4 -= uiDiff;

			if(spell5 <= uiDiff)
				DoCast(me->getVictim(), spell_5);

			if(spell6 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_6);
				spell6 = urand(40000, 60000);
			}
			else
				spell6 -= uiDiff;

			if(spell7 <= uiDiff)
			{
				DoCast(me->getVictim(), spell_7);
				spell7 = 1000;
			}
			else
				spell7 -= uiDiff;
		}

	};

	CreatureAI* GetAI(Creature* creature) const
	{
		return new AzsharaPvP_CasterAI(creature);
	}
};

void AddSC_AzsharaPvP_Caster()
{
	new AzsharaPvP_Caster();
}