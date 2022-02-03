#include "CA.h"
#include "Menu_Log.hpp"

#include <cstdio>

using namespace CATHODE::CA;

__declspec(noinline)
void __stdcall FILE_HASHES::h_verify_integrity(char* package_file_name, unsigned* param_2, unsigned param_3)
{
	//logger.AddLog("[CA::FILE_HASHES::verify_integrity] package_file_name: %s; param_2: 0x%p; param_3: %u\n", package_file_name, param_2, param_3);
	
	verify_integrity(package_file_name, param_2, param_3);
}

/*
__declspec(noinline)
void __stdcall FILE_HASHES::h_terminate_game()
{
	logger.AddLog("[CA::FILE_HASHES::terminate_game] Blocked termination request - is this due to an integrity check failing?!\n");
}
*/

__declspec(noinline)
void __stdcall FILE_HASHES::h_sha1_portable_hash(unsigned char* message_array, unsigned length)
{
	//logger.AddLog("[CA::FILE_HASHES::sha1_portable_hash] message_array: 0x%p; length: %u\n", message_array, length);

	sha1_portable_hash(message_array, length);
}
