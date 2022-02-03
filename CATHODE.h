#pragma once

namespace CATHODE
{
	namespace DataTypes
	{
		/**
		* \brief Holds a pointer to a CATHODE object in memory.
		*/
		typedef unsigned int MemoryPtr;

		/**
		* \brief Holds a pointer to a reference to a CATHODE object in memory.
		*/
		typedef unsigned int MemoryRefPtr;

		/**
		* \brief Holds a pointer to a CATHODE String (a DWORD) in memory.
		*/
		typedef unsigned int String;

		/**
		* \brief Holds the unique GUID of a CATHODE node / parameter.
		*/
		typedef unsigned int ShortGuid;

		/**
		* \brief Holds the ShortGuid of a CATHODE Enum element, and the index of the current value of the Enum.
		*/
		struct Enum
		{
			ShortGuid enum_short_guid = 0;
			int enum_value_index = 0;
		};

		/**
		* \brief Holds three unsigned longs (DWORDs), typically used for colours or positions.
		*/
		struct Vector
		{
			unsigned long v1;
			unsigned long v2;
			unsigned long v3;
		};
	}
}
