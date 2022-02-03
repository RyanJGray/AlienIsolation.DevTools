#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace CATHODE::TriggerInfo
{
	/**
	 * \brief Describes information about a trigger that is active for a CATHODE node.
	 */
	struct TriggerInfo {
		DataTypes::ShortGuid guid; /* The ShortGuid of the node that triggered us. */
		float unk;
		struct MemoryAllocationBase* mem_allocation_base;
		struct TriggerInfo* next_trigger; /* Appears to link to another TriggerInfo object? */
		DataTypes::String name; /* What exactly does this contain? I guessed it was the trigger name, from the datatype. */
		double duration; /* How long the trigger has been active for. */
		short state;
		int reference_count;
	};

	struct MemoryAllocationBase { /* PlaceHolder Structure */
	};
}
