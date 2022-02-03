#include "GAME_LEVEL_MANAGER.h"
#include "Menu_Log.hpp"

#include <stdio.h>

using namespace GAME_LEVEL_MANAGER;

bool g_GameLevelManager_levelLoadInProgress = false;

__declspec(noinline)
int __fastcall GAME_LEVEL_MANAGER::h_get_level_from_name(Instance* _this, void* _EDX, char* level_name)
{
	if (m_instance == nullptr)
	{
		// Save the this pointer for reuse later in the ImGUI menu.
		m_instance = _this;
	}

	//logger.AddLog("[Scaleform::UI::GetLevelPointer] g_getLevelPointer_thisPtr: 0x%p; _this: 0x%p; _EDX: 0x%p; level_name: %s\n", g_getLevelPointer_thisPtr, _this, _EDX, level_name);

	/*if (strcmp(level_name, "Production\\BSP_Torrens") == 0)
	{
		level_name = const_cast<char*>("Production\\Tech_RnD_HzdLab");

		logger.AddLog("[Scaleform::UI::LoadLevel] Switched level_name to %s\n", level_name);
	}*/

	//logger.AddLog("[Scaleform::UI::GetLevelPointer] Got int at offset + 0x2d0: %d\n", *reinterpret_cast<int*>(reinterpret_cast<int>(_this) + 0x2d0));

	const int level = get_level_from_name(_this, level_name);

	//logger.AddLog("[Scaleform::UI::GetLevelPointer] Got ret = %d\n", ret);

	return level;

	// Returning 0 blocks the callback from doing anything remotely related to level loading, there is a check in the original callback function (Callback::GameMenu::LoadLevel)
	// that terminates the callback early if this function returns 0, I assume this is to avoid crashing from unrecognised levels.
	//return 0;
}

__declspec(noinline)
void __fastcall GAME_LEVEL_MANAGER::h_queue_level(Instance* _this, void* _EDX, int level)
{
	//logger.AddLog("[Scaleform::UI::SetNextLevel] _this: 0x%p; _EDX: 0x%p; level_pointer: %d\n", _this, _EDX, level_pointer);

	queue_level(_this, level);
}

__declspec(noinline)
void __fastcall GAME_LEVEL_MANAGER::h_request_next_level(Instance* _this, void* _EDX, bool is_part_of_playlist)
{
	g_GameLevelManager_levelLoadInProgress = true;

	//logger.AddLog("[Scaleform::UI::LoadLevel] _this: 0x%p; _EDX: 0x%p; null_char_string: %s\n", _this, _EDX, null_char_string);

	request_next_level(_this, is_part_of_playlist);
}
