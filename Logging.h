#pragma once

namespace CATHODE::Logging
{
	class Logger
	{
	public:
		Logger() = default;
		~Logger() = default;
	protected:
		static void printFormattedMessage(const char* formatted_message);
		static void formatAndPrintFormattedMessage(char* buffer, const char* format, const char* arg);
	};

	class HavokLogger : Logger
	{
	public:
		static void hijackedPrint(char* buffer, const char* format, const char* havok_physics_object);
	};

	class MusicControllerLogger : Logger
	{
	public:
		static void hijackedPrint_MusicController(char* buffer, const char* format, double* rtpc_value);
		static void hijackedPrint_MusicEvent(char* buffer, const char* format, const char* event_name, double* rtpc_value);
	};

	class AnimationCommandLogger : Logger
	{
	public:
		static void hijackedPrint(char* buffer, const char* format, const char* command_type, const char* command_name);
	};

	class DoorLogger : Logger
	{
	public:
		static void hijackedPrint(char* unknown_1, char* unknown_2, const char* format);
	};

	class AnimationLogger : Logger
	{
	public:
		static void hijackedPrint(char* unknown_1, char* unknown_2, const char* format);
	};

	class CharacterNodeLogger : Logger
	{
	public:
		static void hijackedPrint(char* unknown_1, char* unknown_2, const char* format, char* node_name);
	};

	class ZoneLogger : Logger
	{
	public:
		static void hijackedPrint(char* this_ptr, int zero, const char* format);
	};

	class SpeechLogger : Logger
	{
	public:
		static void hijackedPrint_RequestPriority(char* unknown_1, const char* format, char* unknown_str_1, char* priority, double* timeout);
		static void hijackedPrint_SpeechSize(char* unknown_1, const char* format, unsigned* size);
		static void hijackedPrint_SpacialAudioType(char* unknown_1, const char* format, char* unknown_str_1, char* spacial_audio_type);
		static void hijackedPrint_SpeechEntity(char* this_ptr, const char* format, char* speech_name);
		static void hijackedPrint(char* unknown_1, const char* format);
	};
}