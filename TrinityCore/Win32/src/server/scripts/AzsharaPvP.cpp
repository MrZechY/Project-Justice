#include "AzsharaPvP.h"
#include "Map.h"
#include "InstanceScript.h"
#include "ScriptMgr.h"

class AzsharaPvP : public InstanceMapScript
{
public:
	AzsharaPvP() : InstanceMapScript(AzsharaScriptName, 37) {}

	struct AzsharaPvP_InstanceMapScript : public InstanceScript
	{
		AzsharaPvP_InstanceMapScript(InstanceMap* map) : InstanceScript(map)
		{
		}
	};

	InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
		return new AzsharaPvP_InstanceMapScript(map);
    }
};

void AddSC_AzsharaPvP()
{
	new AzsharaPvP();
}
