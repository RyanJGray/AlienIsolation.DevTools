#pragma once

#include "DevTools.h"
#include "CATHODE.h"

namespace CATHODE::CA
{
	/**
	 * \brief Holds two CATHODE::Vector objects for position and rotation.
	 */
	struct Position
	{
		DataTypes::Vector pos;
		DataTypes::Vector rot;
	};

	namespace FILE_HASHES
	{
		void __stdcall h_verify_integrity(char* package_file_name, unsigned* param_2, unsigned param_3);
		typedef void(__stdcall* t_verify_integrity)(char*, unsigned*, unsigned);
		inline auto verify_integrity = reinterpret_cast<t_verify_integrity>(DEVTOOLS_RELATIVE_ADDRESS(0x00374ca0));

		/*
		void __stdcall h_terminate_game();
		typedef void(__stdcall* t_terminate_game)();
		inline auto terminate_game = reinterpret_cast<t_terminate_game>(DEVTOOLS_RELATIVE_ADDRESS(0x00614770));
		*/

		void __stdcall h_sha1_portable_hash(unsigned char* message_array, unsigned length);
		typedef void(__stdcall* t_sha1_portable_hash)(unsigned char*, unsigned);
		inline auto sha1_portable_hash = reinterpret_cast<t_sha1_portable_hash>(DEVTOOLS_RELATIVE_ADDRESS(0x005335b0));
	}
}
