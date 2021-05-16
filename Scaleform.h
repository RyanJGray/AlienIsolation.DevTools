#pragma once

namespace CATHODE
{
	namespace Scaleform
	{
		namespace Callback
		{
			namespace GameMenu
			{
				inline void* loadLevelThis = nullptr;
				void __fastcall hLoadLevel(void* _this, void* _EDX, char* level_name);
				typedef void(__thiscall* tLoadLevel)(void*, char*);
				inline tLoadLevel LoadLevel = reinterpret_cast<tLoadLevel>(0x008139c0);
			}
		}
		
		namespace UI
		{
			inline void* loadLevelThisPtr = nullptr;
			int __fastcall hLoadLevel(void* _this, void* _EDX, char* level_name);
			typedef int(__thiscall* tLoadLevel)(void*, char*);
			inline tLoadLevel LoadLevel = reinterpret_cast<tLoadLevel>(0x0077b340);

			void __fastcall hHandleLoadRequest(void* _this, void* _EDX, char* param_1);
			typedef void(__thiscall* tHandleLoadRequest)(void*, char*);
			inline tHandleLoadRequest HandleLoadRequest = reinterpret_cast<tHandleLoadRequest>(0x0079a650);

			void __fastcall hLoadLevelUnknownFunc1(int param_1);
			typedef void(__fastcall* tLoadLevelUnknownFunc1)(int);
			inline tLoadLevelUnknownFunc1 LoadLevelUnknownFunc1 = reinterpret_cast<tLoadLevelUnknownFunc1>(0x007bcd40);

			unsigned __fastcall hLoadLevelUnknownFunc2(void* _this, void* _EDX, char* level_name);
			typedef unsigned(__thiscall* tLoadLevelUnknownFunc2)(void*, char*);
			inline tLoadLevelUnknownFunc2 LoadLevelUnknownFunc2 = reinterpret_cast<tLoadLevelUnknownFunc2>(0x007a86c0);

			void __fastcall hLoadLevelUnknownFunc3(void* _this, void* _EDX, int param_1);
			typedef void(__thiscall* tLoadLevelUnknownFunc3)(void*, int);
			inline tLoadLevelUnknownFunc3 LoadLevelUnknownFunc3 = reinterpret_cast<tLoadLevelUnknownFunc3>(0x0077b320);
			
			void __fastcall hDispatchRequestToNodeHandler(void* _this, void* _EDX, unsigned* param_1);
			typedef void(__thiscall* tDispatchRequestToNodeHandler)(void*, unsigned*);
			inline tDispatchRequestToNodeHandler DispatchRequestToNodeHandler = reinterpret_cast<tDispatchRequestToNodeHandler>(0x009c8bd0);
		}
	}
}
