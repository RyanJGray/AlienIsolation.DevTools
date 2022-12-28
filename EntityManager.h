#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace EntityManager
{
	inline void* m_this = nullptr;

	void __fastcall h_jump_to_checkpoint(void* _this, void* _EDX, const char* checkpoint);
	typedef void(__thiscall* t_jump_to_checkpoint)(void*, const char*);
	inline auto jump_to_checkpoint = reinterpret_cast<t_jump_to_checkpoint>(DEVTOOLS_RELATIVE_ADDRESS(0x00522330));
}
