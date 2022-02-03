#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace CATHODE::HackingGame
{
	int __fastcall h_on_custom_method(void* _this, void* _EDX, const DataTypes::ShortGuid& entity, bool& param_2, DataTypes::MemoryPtr& memory_ptr, const DataTypes::MemoryRefPtr& trigger_info);
	typedef int(__thiscall* t_on_custom_method)(void*, const DataTypes::ShortGuid&, bool&, DataTypes::MemoryPtr&, const DataTypes::MemoryRefPtr&);
	// on_custom_method(void*, const ShortGuid&, bool&, MemoryPtr&, const MemoryRefPtr&)
	inline auto on_custom_method = reinterpret_cast<t_on_custom_method>(DEVTOOLS_RELATIVE_ADDRESS(0x00518cd0));

	typedef void(__stdcall* t_trigger_win)(const DataTypes::MemoryPtr& entity, const DataTypes::MemoryRefPtr& trigger_info, double unk);
	inline auto trigger_win = reinterpret_cast<t_trigger_win>(DEVTOOLS_RELATIVE_ADDRESS(0x004d2990));
}
