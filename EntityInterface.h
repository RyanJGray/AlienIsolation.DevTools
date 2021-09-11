#pragma once

#include "CATHODE.h"

namespace CATHODE::EntityInterface
{
	// Stores an EntityInterface pointer that we've stolen from the game calling an EntityInterface member function.
	//static void* g_thisPtr;

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
	bool __fastcall hFindParameterString(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, String& output_ptr);
	typedef bool(__thiscall* tFindParameterString)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, String& output_ptr);
	inline tFindParameterString FindParameterString = reinterpret_cast<tFindParameterString>(0x005991A0);

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
	bool __fastcall hFindParameterBool(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, bool& output_ptr);
	typedef bool(__thiscall* tFindParameterBool)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, bool& output_ptr);
	inline tFindParameterBool FindParameterBool = reinterpret_cast<tFindParameterBool>(0x00630590);

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
	bool __fastcall hFindParameterVector(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, Vector& output_ptr);
	typedef bool(__thiscall* tFindParameterVector)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, Vector& output_ptr);
	inline tFindParameterVector FindParameterVector = reinterpret_cast<tFindParameterVector>(0x008D63C0);

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
	bool __fastcall hFindParameterFloat(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, float& output_ptr);
	typedef bool(__thiscall* tFindParameterFloat)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, float& output_ptr);
	inline tFindParameterFloat FindParameterFloat = reinterpret_cast<tFindParameterFloat>(0x008D6510);

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
	bool __fastcall hFindParameterEnum(void* _this, void* _EDX, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, Enum& output_ptr);
	typedef bool(__thiscall* tFindParameterEnum)(void* _this, const MemoryPtr& entity_ptr, const ShortGuid& shortguid_ptr, Enum& output_ptr);
	inline tFindParameterEnum FindParameterEnum = reinterpret_cast<tFindParameterEnum>(0x008D6620);
}
