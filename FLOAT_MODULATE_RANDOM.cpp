#include "FLOAT_MODULATE_RANDOM.h"

#include "Menu_Log.hpp"

#include <map>

using namespace CATHODE;

int g_totalEntityBehaviourAnimValues = 0;
std::map<int, int*> g_entityBehaviourAnimEnumValues;

void __fastcall FLOAT_MODULATE_RANDOM::hSaveValues(intptr_t* _this, void* /*_EDX*/, bool hasOnThinkParameter,
	bool hasResultParameter, int behaviourAnimEnumValue, float behaviourFrequency, float behaviourFrequencyVariance)
{
	g_entityBehaviourAnimEnumValues.insert(std::make_pair(g_totalEntityBehaviourAnimValues++, reinterpret_cast<int*>(reinterpret_cast<int>(_this) + 0x14)));

	//g_behaviourAnimEnumValue = reinterpret_cast<int*>(reinterpret_cast<int>(_this) + 0x14);
	logger.AddLog("[SaveValues] behaviourAnimEnumValue=%d\n", behaviourAnimEnumValue);

	SaveValues(_this, hasOnThinkParameter, hasResultParameter, behaviourAnimEnumValue, behaviourFrequency, behaviourFrequencyVariance);

	//g_behaviourAnimEnumValue = reinterpret_cast<int*>(reinterpret_cast<int>(_this) + 0x14);
	//logger.AddLog("[SaveValues] g_behaviourAnimEnumValue=%d\n", *g_behaviourAnimEnumValue);
}
