#include "EntityInterface.h"

#include "Menu.h"
#include "Menu_Log.hpp"

#include <map>

using namespace CATHODE;

std::map<ShortGuid, const char*> nodeNameTable;
//std::map<ShortGuid, bool> permittedEnumGuids;
//std::map<ShortGuid, std::pair<Enum, bool>> enumOverrides;
//std::map<ShortGuid, std::pair<Enum, std::string>> entityStates;
//std::map<MemoryPtr, Enum*> enums;
std::map<ShortGuid, bool> boolOverrides;
bool g_shouldOverrideBehaviourAnim = false;
Vector g_lightingColourOverride;
bool g_shouldOverrideLightingColour = false;

bool __fastcall EntityInterface::hFindParameterString(void* _this, void* _EDX, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, String& output_ptr)
{
	bool hasAcquiredRightParameter = false;

	const char* parameterName = "";

	if (Menu::IsInitialised() && shortguid_ptr == 0xAE1048A4 /*&& shortguid_ptr == 0xA9D587D4 && Menu::g_override_ShouldRaiseGunWhileTurning*/)
	{
		nodeNameTable.insert(std::make_pair(shortguid_ptr, "data_file"));
		parameterName = nodeNameTable.find(shortguid_ptr)->second;

		hasAcquiredRightParameter = true;
		logger.AddLog("[EntityInterface] Game - Value of String parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterString(_this, entity_ptr, shortguid_ptr, output_ptr);

	if (hasAcquiredRightParameter)
	{
		logger.AddLog("[EntityInterface] CATHODE - String parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to %s!\n", shortguid_ptr, parameterName, entity_ptr, output_ptr);
	}

	return ret;
}

__declspec(noinline)
bool __fastcall EntityInterface::hFindParameterBool(void* _this, void* /*_EDX*/, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, bool& output_ptr)
{
	/*// GateResourceInterface
	nodeNameTable[0x4B2D970B] = "is_locked";
	// Convo
	nodeNameTable[0xFCDF8ABB] = "playerCanJoin";
	nodeNameTable[0x35F4BE4F] = "everyoneArrived";
	nodeNameTable[0x3DD89128] = "playerJoined";
	nodeNameTable[0x3DB094C3] = "alwaysTalkToPlayerIfPresent";
	// CHR_AimAt, CHR_AimAtCurrentTarget, NPC_TriggerAimRequest
	nodeNameTable[0x42D493F3] = "Raise_gun";
	// CMD_Idle
	nodeNameTable[0xA9D587D4] = "should_raise_gun_while_turning";
	
	const char* parameterName = nodeNameTable[shortguid_ptr];
	*/
	// Steal the EntityInterface this pointer.
	//g_thisPtr = _this;
	bool hasAcquiredRightParameter = false;

	if (Menu::IsInitialised() && shortguid_ptr == 0xB3EB5895 || shortguid_ptr == 0xB41E5E17 /*&& shortguid_ptr == 0xA9D587D4 && Menu::g_override_ShouldRaiseGunWhileTurning*/)
	{
		// If an entry for this override does not yet exist, create one.
		/*if (boolOverrides.find(shortguid_ptr) == boolOverrides.end())
		{
			// Create an override with the default value being true.
			boolOverrides.insert(std::make_pair(shortguid_ptr, true));
		}*/

		boolOverrides.insert(std::make_pair(shortguid_ptr, true));

		hasAcquiredRightParameter = true;
		//logger.AddLog("[EntityInterface] Game - Value of Bool parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterBool(_this, entity_ptr, shortguid_ptr, output_ptr);
	
	/*shortguid_ptr != 0x4B2D970B &&*/ /*shortguid_ptr == 0x346C9BA8*/ 
	if (hasAcquiredRightParameter)
	{
		//logger.AddLog("[EntityInterface] CATHODE - Bool parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to %d!\n", shortguid_ptr, parameterName, entity_ptr, ret);
		
		output_ptr = boolOverrides.find(shortguid_ptr)->second;
		
		//return true;
	}

	return ret;
}

bool __fastcall EntityInterface::hFindParameterVector(void* _this, void* /*_EDX*/, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, Vector& output_ptr)
{
	// LightReference
	//nodeNameTable[0x04742219] = "colour";

	//const char* parameterName = nodeNameTable[shortguid_ptr];

	bool hasAcquiredRightParameter = false;

	if (Menu::IsInitialised() && shortguid_ptr == 0x04742219)
	{
		hasAcquiredRightParameter = true;
		//logger.AddLog("[EntityInterface] Game - Value of Vector parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterVector(_this, entity_ptr, shortguid_ptr, output_ptr);

	if (hasAcquiredRightParameter && g_shouldOverrideLightingColour)
	{
		/*union
		{
			unsigned long rgbVector;
			float rgbVal;
		} colour{};*/

		//logger.AddLog("[EntityInterface] CATHODE - Vector parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to (%f,%f,%f)!\n", shortguid_ptr, parameterName, entity_ptr, rVal, gVal, bVal);

		// Override the RGB Vector to red.
		//colour.rgbVal = 255;
		//output_ptr.v1 = colour.rgbVector;

		//colour.rgbVal = 0;
		//output_ptr.v2 = colour.rgbVector;

		//colour.rgbVal = 0;
		//output_ptr.v3 = colour.rgbVector;

		// Replace the Vector with our overridden one.
		output_ptr = g_lightingColourOverride;

		/*colour.rgbVector = output_ptr.v1;
		const float rVal = colour.rgbVal;
		colour.rgbVector = output_ptr.v2;
		const float gVal = colour.rgbVal;
		colour.rgbVector = output_ptr.v3;
		const float bVal = colour.rgbVal;

		logger.AddLog("[EntityInterface] CATHODE - Vector parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to (%f,%f,%f)!\n", shortguid_ptr, parameterName, entity_ptr, rVal, gVal, bVal);*/
	}

	return ret;
}

__declspec(noinline)
bool __fastcall EntityInterface::hFindParameterFloat(void* _this, void* /*_EDX*/, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, float& output_ptr)
{
	// BloomSettings
	nodeNameTable[0x4BA3CCDB] = "bloom_scale";
	// LensDustSettings
	nodeNameTable[0x5EDC76F] = "DUST_MAX_REFLECTED_BLOOM_INTENSITY";
	// PostprocessingSettings
	nodeNameTable[0x54638299] = "intensity";
	// CameraPlayAnimation
	nodeNameTable[0x821258AF] = "focal_plane_m";
	// NPC_SetRateOfFire
	nodeNameTable[0xFC4AC880] = "MinTimeBetweenShots";
	// NPC_SetFiringRhythm
	nodeNameTable[0x58DFCBF8] = "MinShootingTime";
	nodeNameTable[0x3D5D0279] = "RandomRangeShootingTime";
	nodeNameTable[0x541BB5F7] = "MinNonShootingTime";
	nodeNameTable[0xF92178D0] = "RandomRangeNonShootingTime";
	nodeNameTable[0x22A6EAB0] = "MinCoverNonShootingTime";
	nodeNameTable[0x91B1BEF2] = "RandomRangeCoverNonShootingTime";
	// CAGEAnimation
	nodeNameTable[0x92013583] = "playspeed";

	const char* parameterName = nodeNameTable[shortguid_ptr];
	
	// Steal the EntityInterface this pointer.
	//g_thisPtr = _this;
	bool hasAcquiredRightParameter = false;

	if (Menu::IsInitialised() && shortguid_ptr == 0x92013583 /*(shortguid_ptr == 0xFC4AC880 || shortguid_ptr == 0x58DFCBF8 || shortguid_ptr == 0x3D5D0279 || shortguid_ptr == 0x541BB5F7 || shortguid_ptr == 0xF92178D0 || shortguid_ptr == 0x22A6EAB0 || shortguid_ptr == 0x91B1BEF2)*/)
	{
		hasAcquiredRightParameter = true;
		logger.AddLog("[EntityInterface] Game - Value of Float parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterFloat(_this, entity_ptr, shortguid_ptr, output_ptr);
	
	/*shortguid_ptr != 0x4B2D970B &&*/ /*shortguid_ptr == 0x346C9BA8*/ 
	if (hasAcquiredRightParameter)
	{
		logger.AddLog("[EntityInterface] CATHODE - Float parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to %f!\n", shortguid_ptr, parameterName, entity_ptr, output_ptr);
		
		output_ptr = 4.0f;
		
		//return true;
	}

	return ret;
}

bool __fastcall EntityInterface::hFindParameterEnum(void* _this, void* /*_EDX*/, const MemoryPtr& entity_ptr,
	const ShortGuid& shortguid_ptr, Enum& output_ptr)
{
	/*// CMD_Idle
	nodeNameTable[0x8DAA4E47] = "idle_style";
	// PostprocessingSettings::BlendMode
	nodeNameTable[0xD11717AB] = "blend_mode";
	//permittedEnumGuids[0xD11717AB] = false;
	// FollowCameraModifier::ModiferType
	nodeNameTable[0x6DF6A556] = "modifier_type";
	//permittedEnumGuids[0x6DF6A556] = true;
	// LightAdaptationSettings::adaptation_mechanism
	nodeNameTable[0xCE00E13E] = "adaptation_mechanism";
	// Alliance
	//nodeNameTable[0x48A4AB8A] = "Alliance";
	// FlashScript::Type
	//permittedEnumGuids[0x28953178] = false;
	// FloatModulate::wave_shape
	//permittedEnumGuids[0x9A3FD473] = false;
	// NPC_navmesh_type_monitor::nav_mesh_type
	//permittedEnumGuids[0x8716ACEE] = false;
	// NPC_behaviour_monitor::behaviour
	//permittedEnumGuids[0x1B5B264B] = false;
	// CharacterTypeMonitor::character_class
	nodeNameTable[0xC878CAC6] = "character_class";
	//permittedEnumGuids[0xC878CAC6] = true;
	// FilterIsEnemyOfAllianceGroup::alliance_group
	nodeNameTable[0xA1D92FE0] = "alliance_group";
	// FilterIsCharacterClassCombo::character_classes
	nodeNameTable[0xAF865204] = "character_classes";
	// NPC_SetAlertness::AlertState
	nodeNameTable[0x80451778] = "AlertState";
	//permittedEnumGuids[0x80451778] = false;
	nodeNameTable[0x28F2A5AE] = "behaviour_anim";

	const char* parameterName = nodeNameTable[shortguid_ptr];*/

	bool hasAcquiredRightParameter = false;

	if (Menu::IsInitialised() && (/*shortguid_ptr == 0x80451778 ||*/shortguid_ptr == 0x28F2A5AE))
	{
		hasAcquiredRightParameter = true;
		//logger.AddLog("[EntityInterface] Game - Value of Enum parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) requested!\n", shortguid_ptr, parameterName, entity_ptr);
	}

	const bool ret = FindParameterEnum(_this, entity_ptr, shortguid_ptr, output_ptr);
	
	if (hasAcquiredRightParameter)
	{
		//enums[entity_ptr] = &output_ptr;

		//logger.AddLog("[EntityInterface] CATHODE - Enum parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is set to GUID:0x%X VALUE:%d!\n", shortguid_ptr, parameterName, entity_ptr, output_ptr.enum_short_guid, output_ptr.enum_value_index);

		// Force behaviour_anim to 0 as there is a check to reject real-time anim changes if it is set to 1 (the default).
		//output_ptr.enum_value_index = 0;

		//static std::pair<Enum, std::string> &entityState = entityStates.find(shortguid_ptr)->second;
		//const std::pair<Enum, bool> enumOverride = enumOverrides.find(shortguid_ptr)->second;

		// Does the user want us to override this Enum value?
		/*if (enumOverride.second)
		{
			// Replace the Enum struct with our overridden one.
			output_ptr = enumOverride.first;

			//entityState.first = output_ptr;
			//entityState.second = ALERTNESS_STATE_VALUE;

			logger.AddLog("[EntityInterface] DevTools - Enum parameter (ShortGuid 0x%X, Name %s) on Entity (MemoryPtr 0x%X) is now set to GUID:0x%X VALUE:%s!\n", shortguid_ptr, parameterName, entity_ptr, output_ptr.enum_short_guid, ALERTNESS_STATE_VALUE.c_str());
		}
		else
		{
			entityState.first = output_ptr;
			entityState.second = ALERTNESS_STATE_VALUE;
		}*/
	}

	return ret;
}

