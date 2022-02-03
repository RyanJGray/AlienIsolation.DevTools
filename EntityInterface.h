#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace CATHODE::EntityInterface
{
	// Stores an EntityInterface pointer that we've stolen from the game calling an EntityInterface member function.
	//static void* g_thisPtr;

	typedef void(__cdecl* t_call_triggers)(const DataTypes::MemoryPtr& entity, const DataTypes::ShortGuid& shortguid, const DataTypes::MemoryRefPtr& trigger_info, double unk);
	inline auto call_triggers = reinterpret_cast<t_call_triggers>(DEVTOOLS_RELATIVE_ADDRESS(0x004bd920));

	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, True if the parameter can be found.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<CATHODE::String>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, CATHODE::String&) const
	 */
	bool __fastcall hFindParameterString(void* _this, void* _EDX, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::String& output_ptr);
	typedef bool(__thiscall* tFindParameterString)(void* _this, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::String& output_ptr);
	inline auto FindParameterString = reinterpret_cast<tFindParameterString>(DEVTOOLS_RELATIVE_ADDRESS(0x001991a0));

	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, True if the parameter can be found.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<bool>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, bool&) const
	 */
	bool __fastcall hFindParameterBool(void* _this, void* _EDX, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, bool& output_ptr);
	typedef bool(__thiscall* tFindParameterBool)(void* _this, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, bool& output_ptr);
	inline auto FindParameterBool = reinterpret_cast<tFindParameterBool>(DEVTOOLS_RELATIVE_ADDRESS(0x00230590));

	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, True if the parameter can be found.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<CA::Vector>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, CA::Vector&) const
	 */
	bool __fastcall hFindParameterVector(void* _this, void* _EDX, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::Vector& output_ptr);
	typedef bool(__thiscall* tFindParameterVector)(void* _this, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::Vector& output_ptr);
	inline auto FindParameterVector = reinterpret_cast<tFindParameterVector>(DEVTOOLS_RELATIVE_ADDRESS(0x004d63c0));

	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, True if the parameter can be found.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<float>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, float&) const
	 */
	bool __fastcall hFindParameterFloat(void* _this, void* _EDX, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, float& output_ptr);
	typedef bool(__thiscall* tFindParameterFloat)(void* _this, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, float& output_ptr);
	inline auto FindParameterFloat = reinterpret_cast<tFindParameterFloat>(DEVTOOLS_RELATIVE_ADDRESS(0x004d6510));

	/**
	 * \brief Finds and returns the parameter attached to a CATHODE::Entity object, when passed the parameter's ShortGuid.
	 * \param _this This pointer, must point to an instance of EntityInterface.
	 * \param _EDX Can be ignored, must be included when calling thiscall functions.
	 * \param entity_ptr Pointer to the CATHODE::Entity object that you want a parameter value from.
	 * \param shortguid_ptr Pointer to the parameter's ShortGuid.
	 * \param output_ptr Pointer to the variable that you want the parameter's value to be stored inside.
	 * \return False if the parameter cannot be found, True if the parameter can be found.
	 * \remark Prototype: bool __thiscall CATHODE::EntityInterface::find_parameter<CATHODE::Enum>(CATHODE::MemoryPtr<CATHODE::Entity> const&, CATHODE::ShortGuid const&, CATHODE::Enum&) const
	 */
	bool __fastcall hFindParameterEnum(void* _this, void* _EDX, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::Enum& output_ptr);
	typedef bool(__thiscall* tFindParameterEnum)(void* _this, const DataTypes::MemoryPtr& entity_ptr, const DataTypes::ShortGuid& shortguid_ptr, DataTypes::Enum& output_ptr);
	inline auto FindParameterEnum = reinterpret_cast<tFindParameterEnum>(DEVTOOLS_RELATIVE_ADDRESS(0x004d6620));
}
