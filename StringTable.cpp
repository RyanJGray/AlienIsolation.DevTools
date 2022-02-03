#include "StringTable.h"

#include "Menu_Log.hpp"

using namespace CATHODE;
using namespace CATHODE::DataTypes;

__declspec(noinline)
char* __fastcall CATHODE::StringTable::h_string_from_offset(void* _this, void* _EDX, unsigned int offset)
{
	if (m_instance == nullptr)
	{
		// Save the this pointer for reuse later in the ImGUI menu.
		m_instance = _this;
	}

	// Temporarily store the string that is stored at the passed offset.
	char* ret = string_from_offset(_this, offset);

	//logger.AddLog("[StringTable] String at offset %u is \"%s\".\n", offset, ret);

	return ret;
}
