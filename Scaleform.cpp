#include "Scaleform.h"

#include <cstdio>

using namespace CATHODE::Scaleform;

__declspec(noinline)
void __fastcall Callback::GameMenu::hLoadLevel(void* _this, void* _EDX, char* level_name)
{
	//printf_s("[Callback::GameMenu::LoadLevel] _this: 0x%p; _EDX: 0x%p; level_name: %s\n", _this, _EDX, level_name);

	//level_name = const_cast<char*>("Production\\Tech_MuthrCore");

	//printf_s("[Scaleform::RequestLoadLevel] After: _this: 0x%p; _EDX: 0x%p; param_1: %s\n", _this, _EDX, level_name);
	
	LoadLevel(_this, level_name);
}

__declspec(noinline)
int __fastcall UI::hGetLevelPointer(void* _this, void* _EDX, char* level_name)
{
	// Save the this pointer for reuse later in the ImGUI menu.
	g_getLevelPointer_thisPtr = _this;

	//printf_s("[Scaleform::UI::GetLevelPointer] g_getLevelPointer_thisPtr: 0x%p; _this: 0x%p; _EDX: 0x%p; level_name: %s\n", g_getLevelPointer_thisPtr, _this, _EDX, level_name);

	/*if (strcmp(level_name, "Production\\BSP_Torrens") == 0)
	{
		level_name = const_cast<char*>("Production\\Tech_RnD_HzdLab");

		printf_s("[Scaleform::UI::LoadLevel] Switched level_name to %s\n", level_name);
	}*/

	//printf_s("[Scaleform::UI::GetLevelPointer] Got int at offset + 0x2d0: %d\n", *reinterpret_cast<int*>(reinterpret_cast<int>(_this) + 0x2d0));

	const int ret = GetLevelPointer(_this, level_name);

	//printf_s("[Scaleform::UI::GetLevelPointer] Got ret = %d\n", ret);
	
	return ret;
	
	// Returning 0 blocks the callback from doing anything remotely related to level loading, there is a check in the original callback function (Callback::GameMenu::LoadLevel)
	// that terminates the callback early if this function returns 0, I assume this is to avoid crashing from unrecognised levels.
	//return 0;
}

__declspec(noinline)
void __fastcall UI::hLoadLevel(void* _this, void* _EDX, char* null_char_string)
{
	//printf_s("[Scaleform::UI::LoadLevel] _this: 0x%p; _EDX: 0x%p; null_char_string: %s\n", _this, _EDX, null_char_string);

	LoadLevel(_this, null_char_string);
}

__declspec(noinline)
void __fastcall UI::hLoadLevelUnknownFunc1(int param_1)
{
	//printf_s("[Scaleform::UI::LoadLevelUnknownFunc1] param_1: %d\n", param_1);

	LoadLevelUnknownFunc1(param_1);
}

__declspec(noinline)
unsigned __fastcall UI::hLoadLevelUnknownFunc2(void* _this, void* _EDX, char* level_name)
{
	//printf_s("Yolo swaggins from unknown func 2!\n");
	//printf_s("[Scaleform::UI::LoadLevelUnknownFunc2] _this: 0x%p; _EDX: 0x%p; level_name: %s\n", _this, _EDX, level_name);

	return LoadLevelUnknownFunc2(_this, level_name);
}

__declspec(noinline)
void __fastcall UI::hSetNextLevel(void* _this, void* _EDX, int level_pointer)
{
	//printf_s("[Scaleform::UI::SetNextLevel] _this: 0x%p; _EDX: 0x%p; level_pointer: %d\n", _this, _EDX, level_pointer);

	SetNextLevel(_this, level_pointer);
}

__declspec(noinline)
void __fastcall UI::hDispatchRequestToNodeHandler(void* _this, void* _EDX, unsigned* param_1)
{
	//printf_s("[Scaleform::UI::DispatchRequestToNodeHandler] _this: 0x%p; _EDX: 0x%p; param_1: %s\n", _this, _EDX, reinterpret_cast<char*>(param_1));

	DispatchRequestToNodeHandler(_this, param_1);
}

