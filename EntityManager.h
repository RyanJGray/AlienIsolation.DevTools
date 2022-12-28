#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace EntityManager
{
	void __fastcall h_jump_to_checkpoint(void* _EDX, const char* checkpoint);
	typedef void(__thiscall* t_jump_to_checkpoint)(const char*);
	inline auto jump_to_checkpoint = reinterpret_cast<t_jump_to_checkpoint>(DEVTOOLS_RELATIVE_ADDRESS(0x00522330));
}
