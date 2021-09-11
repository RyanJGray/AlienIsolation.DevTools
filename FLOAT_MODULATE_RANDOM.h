#pragma once

//#include "CATHODE.h"

#include <cstdint>

namespace CATHODE::FLOAT_MODULATE_RANDOM
{
	/*bool __fastcall hGetRequiresState(void* _this, void* _EDX, const MemoryPtr& trigger_info_ptr);
	typedef bool(__thiscall* tGetRequiresState)(void* _this, const MemoryPtr& trigger_info_ptr);
	inline tGetRequiresState GetRequiresState = reinterpret_cast<tGetRequiresState>(0x0059B450);*/

	void __fastcall hSaveValues(intptr_t* _this, void* _EDX, bool hasOnThinkParameter, bool hasResultParameter, int behaviourAnimEnumValue, float behaviourFrequency, float behaviourFrequencyVariance);
	typedef void(__thiscall* tSaveValues)(intptr_t* _this, bool hasOnThinkParameter, bool hasResultParameter, int behaviourAnimEnumValue, float behaviourFrequency, float behaviourFrequencyVariance);
	inline tSaveValues SaveValues = reinterpret_cast<tSaveValues>(0x0059B5F0);
}
