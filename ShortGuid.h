#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace CATHODE::ShortGuid
{
	int __fastcall hShortGuid(DataTypes::ShortGuid* _this, void* _EDX, char* node_name);
	typedef int(__thiscall* tShortGuid)(DataTypes::ShortGuid*, char*);
	inline auto ShortGuid = reinterpret_cast<tShortGuid>(DEVTOOLS_RELATIVE_ADDRESS(0x004bf7f0));
}
