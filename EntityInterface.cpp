#include "EntityInterface.h"

#include "Menu.h"
#include "Menu_Log.hpp"
#include <map>

using namespace CATHODE;

std::map<ShortGuid, const char*> nodeNameTable;

__declspec(noinline)
bool __fastcall EntityInterface::hFindParameterBool(void* _this, void* /*_EDX*/, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, bool& output_ptr)
{
	// GateResourceInterface
	nodeNameTable[0x4B2D970B] = "is_locked";
	// Convo
	nodeNameTable[0xFCDF8ABB] = "playerCanJoin";
	nodeNameTable[0x35F4BE4F] = "everyoneArrived";
	nodeNameTable[0x3DD89128] = "playerJoined";
	nodeNameTable[0x3DB094C3] = "alwaysTalkToPlayerIfPresent";
	
	const char* parameterName = nodeNameTable[shortguid_ptr];
	
	// Steal the EntityInterface this pointer.
	g_thisPtr = _this;
	bool hasAcquiredRightParameter = false;

	if (Menu::IsInitialised() && shortguid_ptr == 0x35F4BE4F)
	{
		hasAcquiredRightParameter = true;
		logger.AddLog("[EntityInterface] Game - Value of Bool parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterBool(_this, entity_ptr, shortguid_ptr, output_ptr);
	
	/*shortguid_ptr != 0x4B2D970B &&*/ /*shortguid_ptr == 0x346C9BA8*/ 
	if (hasAcquiredRightParameter)
	{
		logger.AddLog("[EntityInterface] CATHODE - Bool parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to %d!\n", shortguid_ptr, parameterName, entity_ptr, ret);
		
		output_ptr = true;
		
		return true;
	}

	return ret;
}
