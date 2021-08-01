#pragma once
#include "CATHODE.h"

namespace CATHODE::EntityInterface
{
	// Stores an EntityInterface pointer that we've stolen from the game calling an EntityInterface member function.
	static void* g_thisPtr;
	
	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, else True/False reflecting the parameter's value.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<bool>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, bool&) const
	 */
	bool __fastcall hFindParameterBool(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, bool& output_ptr);
	typedef bool(__thiscall* tFindParameterBool)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, bool& output_ptr);
	inline tFindParameterBool FindParameterBool = reinterpret_cast<tFindParameterBool>(0x00630590);
}
