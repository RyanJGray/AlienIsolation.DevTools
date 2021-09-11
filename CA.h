#pragma once

namespace CATHODE::CA
{
	namespace FILE_HASHES
	{
		void __stdcall h_verify_integrity(char* package_file_name, unsigned* param_2, unsigned param_3);
		typedef void(__stdcall* t_verify_integrity)(char*, unsigned*, unsigned);
		inline t_verify_integrity verify_integrity = reinterpret_cast<t_verify_integrity>(0x00774ca0);

		void __stdcall h_terminate_game();
		typedef void(__stdcall* t_terminate_game)();
		inline t_terminate_game terminate_game = reinterpret_cast<t_terminate_game>(0x00a14770);

		void __stdcall h_sha1_portable_hash(unsigned char* message_array, unsigned length);
		typedef void(__stdcall* t_sha1_portable_hash)(unsigned char*, unsigned);
		inline t_sha1_portable_hash sha1_portable_hash = reinterpret_cast<t_sha1_portable_hash>(0x009335b0);
	}

	namespace LOAD_LEVEL
	{
		void __fastcall hStartGameplayOnLevel(void* _this, void* _EDX, int param_1, char* level_name);
		typedef void(__thiscall* tStartGameplayOnLevel)(void*, int, char*);
		inline tStartGameplayOnLevel StartGameplayOnLevel = reinterpret_cast<tStartGameplayOnLevel>(0x007BCB00);
	}
}

