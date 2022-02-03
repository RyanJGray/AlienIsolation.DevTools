#pragma once

#include "DevTools.h"
//#include "CATHODE.h"

#include <cstdint>

namespace CATHODE::FLOAT_MODULATE_RANDOM
{
	/*bool __fastcall hGetRequiresState(void* _this, void* _EDX, const MemoryPtr& trigger_info_ptr);
	typedef bool(__thiscall* tGetRequiresState)(void* _this, const MemoryPtr& trigger_info_ptr);
	inline auto GetRequiresState = reinterpret_cast<tGetRequiresState>(DEVTOOLS_RELATIVE_ADDRESS(0x0019b450));*/

	void __fastcall hSaveValues(intptr_t* _this, void* _EDX, bool hasOnThinkParameter, bool hasResultParameter, int behaviourAnimEnumValue, float behaviourFrequency, float behaviourFrequencyVariance);
	typedef void(__thiscall* tSaveValues)(intptr_t* _this, bool hasOnThinkParameter, bool hasResultParameter, int behaviourAnimEnumValue, float behaviourFrequency, float behaviourFrequencyVariance);
	inline auto SaveValues = reinterpret_cast<tSaveValues>(DEVTOOLS_RELATIVE_ADDRESS(0x0019b5f0));
}
