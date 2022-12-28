#include "EntityManager.h"
#include "Menu_Log.hpp"

__declspec(noinline)
void __fastcall EntityManager::h_jump_to_checkpoint(void* _this, void* _EDX, const char* checkpoint)
{
	m_this = _this;

	logger.AddLog("CATHODE::EntityManager::jump_to_checkpoint(%s);\n", checkpoint);
	jump_to_checkpoint(_this, checkpoint);
}
