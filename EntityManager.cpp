#include "EntityManager.h"
#include "Menu_Log.hpp"

__declspec(noinline)
void __fastcall EntityManager::h_jump_to_checkpoint(void* /*_EDX*/, const char* checkpoint)
{
	logger.AddLog("[EntityManager] jump_to_checkpoint: %s\n", checkpoint);
	jump_to_checkpoint(checkpoint);
}
