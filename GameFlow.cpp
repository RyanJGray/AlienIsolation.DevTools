#include "GameFlow.h"

#include "Menu_Log.hpp"

using namespace CATHODE;
using namespace CATHODE::DataTypes;

extern bool g_GameLevelManager_levelLoadInProgress;

__declspec(noinline)
void __fastcall GameFlow::h_start_gameplay(void* _this, void* /*_EDX*/, int play_mode, const char* level_name)
{
	logger.AddLog("[GameFlow] Starting gameplay on %s (Play mode: %d)\n", level_name, play_mode);

	g_GameLevelManager_levelLoadInProgress = false;

	start_gameplay(_this, play_mode, level_name);
}
