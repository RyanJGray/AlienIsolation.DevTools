#pragma once
#include <cstdint>

#define DEVTOOLS_DETOURS_ATTACH(original_func, detoured_func) (DetourAttach(&reinterpret_cast<PVOID&>(original_func), detoured_func))
#define DEVTOOLS_DETOURS_DETACH(original_func, detoured_func) (DetourDetach(&reinterpret_cast<PVOID&>(original_func), detoured_func))

// Calculates the relative address for the offset provided.
#define DEVTOOLS_RELATIVE_ADDRESS(offset) (DevTools::GameProcess::GetBaseAddress() + offset)

// Declares a hook for a function which is part of a class (i.e. has a "this" pointer).
#define DEVTOOLS_DECLARE_CLASS_HOOK(return_type, original_func_name, hook_name, typedef_name, func_offset, ...) \
	return_type __fastcall hook_name(void* _this, void* _EDX, __VA_ARGS__); \
	typedef return_type(__thiscall* typedef_name)(void*, __VA_ARGS__); \
	inline auto original_func_name = reinterpret_cast<typedef_name>(DEVTOOLS_RELATIVE_ADDRESS(func_offset));

namespace DevTools
{
	// Singleton class for dynamically fetching the game's process base address.
	class GameProcess
	{
	public:
		static uintptr_t GetBaseAddress();
	};
}
