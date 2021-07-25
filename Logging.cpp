#include "Logging.h"
#include "Menu_Log.hpp"

#include <cstdint>
#include <cstdio>
#include <string>

using namespace CATHODE::Logging;

void Logger::printFormattedMessage(const char* formatted_message)
{
	if (!formatted_message && formatted_message != nullptr)
	{
		logger.AddLog("[Logger] Error - Someone attempted to log a message with a null pointer!\n");
	}
	else
	{
		logger.AddLog("%s\n", formatted_message);
	}
}

void Logger::formatAndPrintFormattedMessage(char* buffer, const char* format, const char* arg)
{
	// Check if we have been provided with a valid buffer to write the result into.
	if (buffer && buffer != nullptr)
	{
		sprintf_s(buffer, 256, format, arg);
		printFormattedMessage(buffer);
	}
	else
	{
		char formatBuffer[256] = {};
		sprintf_s(formatBuffer, 256, format, arg);
		printFormattedMessage(formatBuffer);
	}

	
}

// Because we are hooking in the middle of a function, we need to do some extra work to preserve the state of the registers to avoid access violations.
// We save the return address for the hijacked call to our function, and restore it at the end so the program returns back to the correct place.
// We preserve the state of the volatile registers with PUSHAD, and we restore them in our custom function epilogue with POPAD.
// We also preserve the state of the EFLAGS with PUSHFD, and restore them with POPFD.
// __declspec(naked) instructs the compiler that we do not need it to autogenerate a function prologue and epilogue in assembly, as we will write our own.
long returnAddress;

__declspec(naked)
void HavokLogger::hijackedPrint(char* /*buffer*/, const char* format, const char* havok_physics_object)
{
	// Our custom prologue.
	__asm {
		// Save the return address to our variable.
		mov eax, [ebp+4]
		mov returnAddress, eax

		// Save the stack pointer, all general registers and EFLAGS.
		push ebp
		mov ebp, esp
		pushad
		pushfd
	}
	
	formatAndPrintFormattedMessage(nullptr, format, havok_physics_object);
	
	// Our custom epilogue.
	__asm {
		// Restore all EFLAGS, general registers and the stack pointer.
		popfd
		popad
		mov esp, ebp
		pop ebp
	}

	__asm {
		// Restore the return address and return to fully restore the stack back to its original condition.
		mov eax, [returnAddress]
		mov [ebp+4], eax
		ret
	}
}

void MusicControllerLogger::hijackedPrint_MusicController(char* buffer, const char* format, double* rtpc_value)
{
	sprintf_s(buffer, 256, format, rtpc_value);
	printFormattedMessage(buffer);
}

void MusicControllerLogger::hijackedPrint_MusicEvent(char* buffer, const char* format, const char* event_name,
                                                     double* rtpc_value)
{
	sprintf_s(buffer, 256, format, event_name, rtpc_value);
	printFormattedMessage(buffer);
}

void AnimationCommandLogger::hijackedPrint(char* buffer, const char* format, const char* command_type,
										   const char* command_name)
{
	sprintf_s(buffer, 256, format, command_type, command_name);
	printFormattedMessage(buffer);
}

void DoorLogger::hijackedPrint(char* /*unknown_1*/, char* /*unknown_2*/, const char* format)
{
	formatAndPrintFormattedMessage(nullptr, format, "");
}

void AnimationLogger::hijackedPrint(char* /*unknown_1*/, char* /*unknown_2*/, const char* format)
{
	formatAndPrintFormattedMessage(nullptr, format, "");
}

void CharacterNodeLogger::hijackedPrint(char* /*unknown_1*/, char* /*unknown_2*/, const char* format, char* node_name)
{
	formatAndPrintFormattedMessage(nullptr, format, node_name);
}

void ZoneLogger::hijackedPrint(char* /*this_ptr*/, int /*zero*/, const char* format)
{
	formatAndPrintFormattedMessage(nullptr, format, "");
}

void SpeechLogger::hijackedPrint_RequestPriority(char* /*unknown_1*/, const char* format, char* unknown_str_1, char* priority, double* timeout)
{
	char formatBuffer[256] = {};
	sprintf_s(formatBuffer, 256, format, unknown_str_1, priority, timeout);
	printFormattedMessage(formatBuffer);
}

void SpeechLogger::hijackedPrint_SpeechSize(char* /*unknown_1*/, const char* format, unsigned* size)
{
	char formatBuffer[256] = {};
	sprintf_s(formatBuffer, 256, format, size);
	printFormattedMessage(formatBuffer);
}

void SpeechLogger::hijackedPrint_SpacialAudioType(char* /*unknown_1*/, const char* format, char* unknown_str_1, char* spacial_audio_type)
{
	char formatBuffer[256] = {};
	sprintf_s(formatBuffer, 256, format, unknown_str_1, spacial_audio_type);
	printFormattedMessage(formatBuffer);
}

void SpeechLogger::hijackedPrint_SpeechEntity(char* this_ptr, const char* format, char* speech_name)
{
	formatAndPrintFormattedMessage(nullptr, format, speech_name);
}

void SpeechLogger::hijackedPrint(char* /*unknown_1*/, const char* format)
{
	formatAndPrintFormattedMessage(nullptr, format, "");
}
