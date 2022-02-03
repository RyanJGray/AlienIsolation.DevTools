#include "HackingGame.h"

#include "TriggerInfo.h"
#include "Entity.h"
#include "EntityInterface.h"
#include "StringTable.h"

#include "Menu_Log.hpp"

using namespace CATHODE;
using namespace CATHODE::DataTypes;

//typedef bool(__thiscall* t_unlock)(void*, MemoryPtr&, const MemoryRefPtr&);

int __fastcall CATHODE::HackingGame::h_on_custom_method(void* _this, void* _EDX, const ShortGuid& shortguid_ptr, bool& param_2, MemoryPtr& entity_ptr, const MemoryRefPtr& trigger_info_ptr)
{
	/*
	TriggerInfo::TriggerInfo* trigger = (TriggerInfo::TriggerInfo*)trigger_info_ptr;

	char* trigger_name = StringTable::string_from_offset(StringTable::m_instance, trigger->name);

	logger.AddLog("[HackingGame] trigger->guid = 0x%X\n", trigger->guid);
	logger.AddLog("[HackingGame] trigger->name = 0x%X\n", trigger->name);
	logger.AddLog("[HackingGame] trigger->trigger_name = %s\n", trigger_name);
	logger.AddLog("[HackingGame] trigger->reference_count = %d\n", trigger->reference_count);
	logger.AddLog("[HackingGame] trigger->unk = %f\n", trigger->unk);
	logger.AddLog("[HackingGame] trigger->duration = %f\n", trigger->duration);
	logger.AddLog("[HackingGame] trigger->state = %d\n", trigger->state);

	TriggerInfo::TriggerInfo* next = trigger->next_trigger;
	if (next != nullptr)
	{
		trigger_name = StringTable::string_from_offset(StringTable::m_instance, next->name);

		logger.AddLog("[HackingGame] trigger->next_trigger->guid = 0x%X;", next->guid);
		logger.AddLog("[HackingGame] trigger->next_trigger->name = 0x%X\n", next->name);
		logger.AddLog("[HackingGame] trigger->next_trigger->trigger_name = %s\n", trigger_name);
		logger.AddLog("[HackingGame] trigger->next_trigger->reference_count = %d\n", next->reference_count);
		logger.AddLog("[HackingGame] trigger->next_trigger->unk = %f\n", next->unk);
		logger.AddLog("[HackingGame] trigger->next_trigger->duration = %f\n", next->duration);
		logger.AddLog("[HackingGame] trigger->next_trigger->state = %d\n", next->state);
	}
	*/
	//ShortGuid& win_target_guid = *(ShortGuid*)0x017534C8;

	//EntityInterface::call_triggers(entity, win_target_guid, trigger_info, 0.0);
	//trigger_win(entity_ptr, trigger_info_ptr, 0.0);

	// unlock(void*, MemoryPtr&, const MemoryRefPtr&)
	//t_unlock unlock = reinterpret_cast<t_unlock>((long*)_this + 0x590);

	//bool bVar1 = false;
	//bVar1 = unlock(_this, memory_ptr, trigger_info);

	// unlock
	/*if (entity == 0x8788E6A4)
	{
		logger.AddLog("[HackingGame] unlock() skipped\n");
		param_2 = false;
		return true;
	}*/

	return on_custom_method(_this, shortguid_ptr, param_2, entity_ptr, trigger_info_ptr);
}
