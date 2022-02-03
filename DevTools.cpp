#include "DevTools.h"
#include <cstdint>
#include <stdio.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace DevTools;

/*
	Pointer to the base address of AI.EXE's process.

	We can bypass the game's ASLR protection, by getting the randomised base address of the game, and applying the offset
	of the function / memory address that we are looking for.
	ASLR on Windows just randomises the image's base address, not the order in which functions appear in memory.
*/
uintptr_t m_game_imageBaseAddress = NULL;

uintptr_t GameProcess::GetBaseAddress()
{
	// If we haven't got the image base address yet, set it here.
	if (m_game_imageBaseAddress == NULL)
	{
		m_game_imageBaseAddress = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));
	}

	return m_game_imageBaseAddress;
}
